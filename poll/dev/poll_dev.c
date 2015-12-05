#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/wait.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#define POLL_DEV_NAME		"polldev"
#define POLL_DEV_MAJOR		240

#define POLL_WRITE_ADDR		0x0378
#define POLL_READ_ADDR		0x0379
#define POLL_CTRL_ADDR		0x037a

#define POLL_IRQ		7
#define POLL_IRQ_ENABLE_MASK	0x10

#define POLL_BUF_MAX		64

static DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read);

#define  MAX_QUEUE_CNT		128

static uint8_t ReadQ[MAX_QUEUE_CNT];
static unsigned long ReadQCount;
static unsigned long ReadQHead;
static unsigned long ReadQTail;

static irqreturn_t poll_interrupt(int irq, void *dev_id)
{
	unsigned long flags;

	local_save_flags(flags);
	local_irq_disable();

	if (ReadQCount < MAX_QUEUE_CNT) {
		ReadQ[ReadQHead] = (unsigned long)inb(POLL_READ_ADDR);
		ReadQHead = (ReadQHead + 1) % MAX_QUEUE_CNT;
		ReadQCount++;
	}

	local_irq_restore(flags);

	wake_up_interruptible(&WaitQueue_Read);

	return IRQ_HANDLED;
}

static int poll_open(struct inode *inode, struct file *filp)
{
	if (!request_irq(POLL_IRQ, poll_interrupt, IRQF_DISABLED,
				POLL_DEV_NAME, NULL))
		outb(POLL_IRQ_ENABLE_MASK, POLL_CTRL_ADDR);

	return 0;
}

static ssize_t poll_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	unsigned long flags;
	int realmax;
	int loop;
	int retstate;

	if ((!ReadQCount) && (filp->f_flags & O_NONBLOCK))
		return -EAGAIN;

	retstate = wait_event_interruptible(WaitQueue_Read, ReadQCount);
	if (retstate)
		return retstate;

	local_save_flags(flags);
	local_irq_disable();

	realmax = 0;
	if (ReadQCount > 0) {
		if (ReadQCount <= count)
			realmax = ReadQCount;
		else; 
			realmax = count;

		for (loop = 0; loop < realmax; loop++) {
			put_user(ReadQ[ReadQTail], (char *)&buf[loop]);
			ReadQTail = (ReadQTail + 1) % MAX_QUEUE_CNT;
			ReadQCount--;
		}
	}

	local_irq_restore(flags);

	return realmax;
}

static ssize_t poll_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	uint8_t status;
	int loop;

	for (loop = 0; loop < count; loop++) {
		get_user(status, (char *)&buf[loop]);
		outb(status, POLL_WRITE_ADDR);
	}

	return count;
}

static unsigned int poll_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;

	poll_wait(filp, &WaitQueue_Read, wait);

	if (ReadQCount > 0)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static int poll_release(struct inode *inode, struct file *filp)
{
	outb(0x00, POLL_CTRL_ADDR);
	free_irq(POLL_IRQ, NULL);

	return 0;
}

static struct file_operations poll_fops = {
	.owner = THIS_MODULE,
	.read = poll_read,
	.write = poll_write,
	.poll = poll_poll,
	.open = poll_open,
	.release = poll_release,
};

static int poll_init(void)
{
	int result;

	result = register_chrdev(POLL_DEV_MAJOR, POLL_DEV_NAME, &poll_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void poll_exit(void)
{
	unregister_chrdev(POLL_DEV_MAJOR, POLL_DEV_NAME);
}

module_init(poll_init);
module_exit(poll_exit);

MODULE_LICENSE("Dual BSD/GPL");

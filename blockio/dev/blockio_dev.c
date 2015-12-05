#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define BLOCKIO_DEV_NAME	"blockiodev"
#define BLOCKIO_DEV_MAJOR	240

#define BLOCKIO_WRITE_ADDR	0x0378
#define BLOCKIO_READ_ADDR	0x0379
#define BLOCKIO_CTRL_ADDR	0x037A

#define BLOCKIO_IRQ		7
#define BLOCKIO_IRQ_ENABLE_MASK	0x10

#define BLOCKIO_BUFF_MAX	64

typedef struct {
	unsigned long time;
} __attribute__((packed)) R_BLOCKIO_INFO;

static R_BLOCKIO_INFO intbuffer[BLOCKIO_BUFF_MAX];
static int intcount;

static DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read);

static void blockio_clear(void)
{
	int lp;

	for (lp = 0; lp < BLOCKIO_BUFF_MAX; lp++)
		intbuffer[lp].time = 0;

	intcount = 0;
}

static irqreturn_t blockio_interrupt(int irq, void *dev_id)
{
	if (intcount < BLOCKIO_BUFF_MAX) {
		intbuffer[intcount].time = get_jiffies_64();
		intcount++;
	}

	wake_up_interruptible(&WaitQueue_Read);

	return IRQ_HANDLED;
}

static int blockio_open(struct inode *inode, struct file *filp)
{
	if (!request_irq(BLOCKIO_IRQ, blockio_interrupt, IRQF_DISABLED,
				BLOCKIO_DEV_NAME, NULL))
		outb(BLOCKIO_IRQ_ENABLE_MASK, BLOCKIO_CTRL_ADDR);

	blockio_clear();

	return 0;
}

static ssize_t blockio_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	int readcount;
	char *ptrdata;
	int loop;
	int ret;

	if (!intcount  && (filp->f_flags & O_NONBLOCK))
		return -EAGAIN;

	ret = wait_event_interruptible(WaitQueue_Read, intcount);
	if (ret)
		return ret;

	readcount = count / sizeof(R_BLOCKIO_INFO);
	if (readcount > intcount)
		readcount = intcount;

	ptrdata = (char *)&intbuffer[0];

	for (loop = 0; loop < readcount * sizeof(R_BLOCKIO_INFO); loop++)
		put_user(ptrdata[loop], (char *)&buf[loop]);

	return readcount * sizeof(R_BLOCKIO_INFO);
}

static ssize_t blockio_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	uint8_t status;
	int loop;

	blockio_clear();

	for (loop = 0; loop < count; loop++) {
		get_user(status, (char *)buf);
		outb(status, BLOCKIO_WRITE_ADDR);
	}

	return count;
}

static int blockio_release(struct inode *inode, struct file *filp)
{
	outb(0x00, BLOCKIO_CTRL_ADDR);
	free_irq(BLOCKIO_IRQ, NULL);

	return 0;
}

static struct file_operations blockio_fops = {
	.owner = THIS_MODULE,
	.read = blockio_read,
	.write = blockio_write,
	.open = blockio_open,
	.release = blockio_release,
};

static int blockio_init(void)
{
	int result;

	result = register_chrdev(BLOCKIO_DEV_MAJOR, BLOCKIO_DEV_NAME,
			&blockio_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void blockio_exit(void)
{
	unregister_chrdev(BLOCKIO_DEV_MAJOR, BLOCKIO_DEV_NAME);
}

module_init(blockio_init);
module_exit(blockio_exit);

MODULE_LICENSE("Dual BSD/GPL");

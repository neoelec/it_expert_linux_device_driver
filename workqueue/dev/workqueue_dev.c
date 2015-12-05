#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#define DEV_NAME		"workqueue"
#define DEV_MAJOR		240

#define PRN_ADDRESS_RANGE	3
#define PRN_ADDRESS		0x0378
#define PRN_ADDRESS_STATE	0x0379
#define PRN_ADDRESS_CTRL	0x037a

#define PRN_IRQ_ENABLE_MASK	0x10
#define PRN_IRQ			7

static void call_workqueuefunc(struct work_struct *data);

static DECLARE_WAIT_QUEUE_HEAD(waitqueue_read);
static DECLARE_WORK(work_queue, call_workqueuefunc);

static void call_workqueuefunc(struct work_struct *data)
{
	pr_info("[WORK QUEUE] Enter workqueue function\n");
}

static irqreturn_t workqueue_interrupt(int irq, void *dev_id)
{
	pr_info("[WORK QUEUE] Enter workqueue_interrupt in interrupt\n");

	schedule_work(&work_queue);
	wake_up_interruptible(&waitqueue_read);
	pr_info("[WORK QUEUE] After schedule_work interrupt\n");

	return IRQ_HANDLED;
}

static ssize_t workqueue_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	int ret;

	pr_info("[WORK QUEUE] Enter %s in read\n", __func__);
	schedule_work(&work_queue);

	ret = wait_event_interruptible(waitqueue_read, count);
	if (ret)
		return ret;

	pr_info("[WORK QUEUE] After schedule_work in read\n");

	return 1;
}

static ssize_t workqueue_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	pr_info("[WORK QUEUE] Enter %s in write\n", __func__);
	schedule_work(&work_queue);
	pr_info("[WORK QUEUE] After schedule_work in write\n");

	return 1;
}

static int workqueue_open(struct inode *inode, struct file *filp)
{
	if (!request_irq(PRN_IRQ, workqueue_interrupt, IRQF_DISABLED,
				DEV_NAME, NULL))
		outb(PRN_IRQ_ENABLE_MASK, PRN_ADDRESS_CTRL);

	try_module_get(THIS_MODULE);

	return 0;
}

static int workqueue_release(struct inode *inode, struct file *filp)
{
	outb(0x00, PRN_ADDRESS_CTRL);
	free_irq(PRN_IRQ, NULL);

	module_put(THIS_MODULE);

	return 0;
}

static struct file_operations workqueue_fops = {
	.owner = THIS_MODULE,
	.read = workqueue_read,
	.write = workqueue_write,
	.open = workqueue_open,
	.release = workqueue_release,
};

static int workqueue_init(void)
{
	int result;

	result = register_chrdev(DEV_MAJOR, DEV_NAME, &workqueue_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void workqueue_exit(void)
{
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
}

module_init(workqueue_init);
module_exit(workqueue_exit);

MODULE_LICENSE("Dual BSD/GPL");

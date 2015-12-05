#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/fs.h>

#define CALL_DEV_NAME		"calldev"
#define CALL_DEV_MAJOR		240

static int call_open(struct inode *inode, struct file *filp)
{
	int num = MINOR(inode->i_rdev);

	pr_info("call open -> minor : %d\n", num);

	return 0;
}

static loff_t call_llseek(struct file *filp, loff_t off, int whence)
{
	pr_info("call llseek -> off : %08zx,  whence : %08x\n",
			(size_t)off, whence);

	return 0x23;
}

static ssize_t call_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	pr_info("call read -> buf : %p, count : %08zx\n", buf, count);

	return 0x33;
}

static ssize_t call_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	pr_info("call write -> buf : %p, count : %08zx\n", buf, count);

	return 0x43;
}

static long call_unlocked_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	pr_info("call ioctl -> cmd : %08x, arg : %08lx\n", cmd, arg);

	return 0x53;
}

static int call_release(struct inode *inode, struct file *filp)
{
	pr_info("call release\n");

	return 0;
}

static struct file_operations call_fops = {
	.owner = THIS_MODULE,
	.llseek = call_llseek,
	.read = call_read,
	.write = call_write,
	.unlocked_ioctl = call_unlocked_ioctl,
	.open = call_open,
	.release = call_release,
};

static int call_init(void)
{
	int result;

	pr_info("call %s\n", __func__);

	result = register_chrdev(CALL_DEV_MAJOR, CALL_DEV_NAME, &call_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void call_exit(void)
{
	pr_info("call %s\n", __func__);

	unregister_chrdev(CALL_DEV_MAJOR, CALL_DEV_NAME);
}

module_init(call_init);
module_exit(call_exit);

MODULE_LICENSE("Dual BSD/GPL");

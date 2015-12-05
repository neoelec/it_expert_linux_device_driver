#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define MINOR_DEV_NAME		"minordev"
#define MINOR_DEV_MAJOR		240
#define MINOR_WRITE_ADDR	0x0378
#define MINOR_READ_ADDR		0x0379

static int minor0_open(struct inode *inode, struct file *filp)
{
	pr_info("call %s\n", __func__);

	return 0;
}

static ssize_t minor0_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	uint8_t status;
	int loop;

	for (loop = 0; loop < count; loop++) {
		get_user(status, (char *)&buf[loop]);
		outb(status, MINOR_WRITE_ADDR);
	}

	return count;
}

static int minor0_release(struct inode *inode, struct file *filp)
{
	pr_info("call %s\n", __func__);

	return 0;
}

static struct file_operations minor0_fops = {
	.owner = THIS_MODULE,
	.write = minor0_write,
	.open = minor0_open,
	.release = minor0_release,
};

static int minor1_open(struct inode *inode, struct file *filp)
{
	pr_info("call %s\n", __func__);

	return 0;
}

static ssize_t minor1_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	uint8_t status;
	int loop;

	for (loop = 0; loop < count; loop++) {
		status = inb(MINOR_READ_ADDR);
		put_user(status, (char *)&buf[loop]);
	}

	return count;
}

static int minor1_release(struct inode *inode, struct file *filp)
{
	pr_info("call %s\n", __func__);

	return 0;
}

static struct file_operations minor1_fops = {
	.owner = THIS_MODULE,
	.read = minor1_read,
	.open = minor1_open,
	.release = minor1_release,
};

static int minor_open(struct inode *inode, struct file *filp)
{
	pr_info("call %s\n", __func__);

	switch (MINOR(inode->i_rdev)) {
	case 1:
		filp->f_op = &minor0_fops;
		break;
	case 2:
		filp->f_op = &minor1_fops;
		break;
	default:
		return -ENXIO;
	}

	if (filp->f_op && filp->f_op->open)
		return filp->f_op->open(inode, filp);

	return 0;
}

static struct file_operations minor_fops = {
	.owner = THIS_MODULE,
	.open = minor_open,
};

static int minor_init(void)
{
	int result;

	result = register_chrdev(MINOR_DEV_MAJOR, MINOR_DEV_NAME,
			&minor_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void minor_exit(void)
{
	unregister_chrdev(MINOR_DEV_MAJOR, MINOR_DEV_NAME);
}

module_init(minor_init);
module_exit(minor_exit);

MODULE_LICENSE("Dual BSD/GPL");

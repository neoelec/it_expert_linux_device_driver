#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define RDWR_DEV_NAME		"rdwrdev"
#define RDWR_DEV_MAJOR		240

#define RDWR_WRITE_ADDR		0x0378
#define RDWR_READ_ADDR		0x0379

static int rdwr_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t rdwr_read(struct file *filp, char *buf, size_t count,
		loff_t *f_pos)
{
	uint8_t status;
	int loop;

	for (loop = 0; loop < count; loop++) {
		status = inb(RDWR_READ_ADDR);
		put_user(status, (char *)&buf[loop]);
	}

	return count;
}

static ssize_t rdwr_write(struct file *filp, const char *buf, size_t count,
		loff_t *f_pos)
{
	uint8_t status;
	int loop;

	for (loop = 0; loop < count; loop++) {
		get_user(status, (char *)buf);
		outb(status, RDWR_WRITE_ADDR);
	}

	return count;
}

static int rdwr_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations rdwr_fops = {
	.owner = THIS_MODULE,
	.read = rdwr_read,
	.write = rdwr_write,
	.open = rdwr_open,
	.release = rdwr_release,
};

static int rdwr_init(void)
{
	int result;

	result = register_chrdev(RDWR_DEV_MAJOR, RDWR_DEV_NAME,
			&rdwr_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void rdwr_exit(void)
{
	unregister_chrdev(RDWR_DEV_MAJOR, RDWR_DEV_NAME);
}

module_init(rdwr_init);
module_exit(rdwr_exit);

MODULE_LICENSE("Dual BSD/GPL");

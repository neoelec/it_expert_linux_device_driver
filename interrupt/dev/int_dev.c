#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define INT_DEV_NAME		"intdev"
#define INT_DEV_MAJOR		240

#define INT_WRITE_ADDR		0x0378
#define INT_READ_ADDR		0x0379
#define INT_CTRL_ADDR		0x037a

#define PRINT_IRQ		7
#define PRINT_IRQ_ENABLE_MASK	0x10

#define INT_BUFF_MAX		64

typedef struct {
	unsigned long time;
} __attribute__((packed)) R_INT_INFO;

static R_INT_INFO intbuffer[INT_BUFF_MAX];
static int intcount;

static void int_clear(void)
{
	int lp;

	for (lp = 0; lp < INT_BUFF_MAX; lp++)
		intbuffer[lp].time = 0;

	intcount = 0;
}

static irqreturn_t int_interrupt(int irq, void *dev_id)
{
	if (intcount < INT_BUFF_MAX) {
		intbuffer[intcount].time = get_jiffies_64();
		intcount++;
	}

	return IRQ_HANDLED;
}

static int int_open(struct inode *inode, struct file *filp)
{
	if (!request_irq(PRINT_IRQ, int_interrupt, IRQF_DISABLED,
				INT_DEV_NAME, NULL))
		outb(PRINT_IRQ_ENABLE_MASK, INT_CTRL_ADDR);

	int_clear();

	return 0;
}

static ssize_t int_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	int readcount;
	char *ptrdata;
	int loop;

	readcount = count / sizeof(R_INT_INFO);
	if (readcount >intcount)
		readcount = intcount;

	ptrdata = (char *)&intbuffer[0];

	for (loop = 0; loop < readcount * sizeof(R_INT_INFO); loop++)
		put_user(ptrdata[loop], (char *)&buf[loop]);

	return readcount * sizeof(R_INT_INFO);
}

static ssize_t int_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	uint8_t status;
	int loop;

	int_clear();

	for (loop = 0; loop < count; loop++) {
		get_user(status, (char *)buf);
		outb(status, INT_WRITE_ADDR);
	}

	return count;
}

static int int_release(struct inode *inode, struct file *filp)
{
	outb(0x00, INT_CTRL_ADDR);
	free_irq(PRINT_IRQ, NULL);

	return 0;
}

static struct file_operations int_fops = {
	.owner = THIS_MODULE,
	.read = int_read,
	.write = int_write,
	.open = int_open,
	.release = int_release,
};

static int int_init(void)
{
	int result;

	result = register_chrdev(INT_DEV_MAJOR, INT_DEV_NAME, &int_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void int_exit(void)
{
	unregister_chrdev(INT_DEV_MAJOR, INT_DEV_NAME);
}

module_init(int_init);
module_exit(int_exit);

MODULE_LICENSE("Dual BSD/GPL");

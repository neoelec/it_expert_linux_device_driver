#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <asm/uaccess.h>

static struct proc_dir_entry *sumproc_root_fp;
static struct proc_dir_entry *sumproc_val1_fp;
static struct proc_dir_entry *sumproc_val2_fp;
static struct proc_dir_entry *sumproc_result_fp;

static char sumproc_str1[PAGE_SIZE - 80] = { '0', };
static char sumproc_str2[PAGE_SIZE - 80] = { '0', };

static int show_sumproc_val(struct seq_file *m, void *v)
{
	seq_printf(m, "Value = [%s]\n", (char *)m->private);

	return 0;
}

static int open_sumproc_val(struct inode *inode, struct file *filp)
{
	return single_open(filp, show_sumproc_val,
			PDE_DATA(file_inode(filp)));
}

static ssize_t write_sumproc_val(struct file *filp, const char __user *buf,
	size_t count, loff_t *f_pos)
{
	char *data;
	ssize_t len;

	data = PDE_DATA(file_inode(filp));

	if (copy_from_user(data, buf, count))
		return -EFAULT;

	data[count] = '\0';
	len = strlen(data);
	if (data[len - 1] == '\n')
		data[--len] = '\0';

	return count;
}

static int show_sumproc_result(struct seq_file *m, void *v)
{
	int a, b, sum;

	a = simple_strtoul(sumproc_str1, NULL, 10);
	b = simple_strtoul(sumproc_str2, NULL, 10);
	sum = a + b;

	seq_printf(m, "Result [%d + %d = %d]\n", a, b, sum);

	return 0;
}

static int open_sumproc_result(struct inode *inode, struct file *filp)
{
	return single_open(filp, show_sumproc_result, NULL);
}

static struct file_operations sumproc_val1_fops = {
	.owner = THIS_MODULE,
	.open = open_sumproc_val,
	.read = seq_read,
	.write = write_sumproc_val,
	.release = single_release,
};

static struct file_operations sumproc_val2_fops = {
	.owner = THIS_MODULE,
	.open = open_sumproc_val,
	.read = seq_read,
	.write = write_sumproc_val,
	.release = single_release,
};

static struct file_operations sumproc_result_fops = {
	.owner = THIS_MODULE,
	.open = open_sumproc_result,
	.read = seq_read,
	.release = single_release,
};

static int sumproc_init(void)
{
	sumproc_root_fp = proc_mkdir("sumproc", 0);

	sumproc_val1_fp = proc_create_data("val1", S_IFREG | S_IRUSR | S_IWUSR,
			sumproc_root_fp, &sumproc_val1_fops, sumproc_str1);

	sumproc_val2_fp = proc_create_data("val2", S_IFREG | S_IRUSR | S_IWUSR,
			sumproc_root_fp, &sumproc_val2_fops, sumproc_str2);

	sumproc_result_fp = proc_create("result", S_IFREG | S_IRUSR,
			sumproc_root_fp, &sumproc_result_fops);

	return 0;
}

static void sumproc_exit(void)
{
	remove_proc_entry("result", sumproc_root_fp);
	remove_proc_entry("val2", sumproc_root_fp);
	remove_proc_entry("vla1", sumproc_root_fp);
	remove_proc_entry("sumproc", 0);
}

module_init(sumproc_init);
module_exit(sumproc_exit);

MODULE_LICENSE("Dual BSD/GPL");

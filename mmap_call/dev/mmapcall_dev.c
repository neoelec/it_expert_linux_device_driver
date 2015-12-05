#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>

#define MMAPCALL_DEV_NAME	"mmapcall"
#define MMAPCALL_DEV_MAJOR	240

static int mmapcall_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mmapcall_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mmapcall_mmap(struct file *filp, struct vm_area_struct *vma)
{
	pr_info("vm_pgoff    [%08lx]\n", vma->vm_pgoff << PAGE_SHIFT);
	pr_info("vm_start    [%08lx]\n", vma->vm_start);
	pr_info("vm_end      [%08lx]\n", vma->vm_end);
	pr_info("vm_flags    [%08lx]\n", vma->vm_flags);

	return -EAGAIN;
}

static struct file_operations mmapcall_fops = {
	.owner = THIS_MODULE,
	.open = mmapcall_open,
	.release = mmapcall_release,
	.mmap = mmapcall_mmap,
};

static int mmapcall_init(void)
{
	int result;

	result = register_chrdev(MMAPCALL_DEV_MAJOR, MMAPCALL_DEV_NAME,
			&mmapcall_fops);
	if (result < 0) {
		pr_err("register_chrdev failed\n");
		return result;
	}

	return 0;
}

static void mmapcall_exit(void)
{
	unregister_chrdev(MMAPCALL_DEV_MAJOR, MMAPCALL_DEV_NAME);
}

module_init(mmapcall_init);
module_exit(mmapcall_exit);

MODULE_LICENSE("Dual BSD/GPL");

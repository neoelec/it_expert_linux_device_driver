#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

static void kmalloc_test(void)
{
	char *buff;

	pr_info("kmalloc test\n");

	buff = kmalloc(1204, GFP_KERNEL);
	if (buff) {
		sprintf(buff, "test memory\n");
		pr_info("%s", buff);
		kfree(buff);
	}

	buff = kmalloc(32 * PAGE_SIZE, GFP_KERNEL);
	if (buff) {
		pr_info("Big Memory OK\n");
		kfree(buff);
	}
}

static void vmalloc_test(void)
{
	char *buff;

	pr_info("vmalloc test\n");

	buff = vmalloc(33 * PAGE_SIZE);
	if (buff) {
		sprintf(buff, "vmalloc test ok\n");
		pr_info("%s", buff);
		vfree(buff);
	}
}

static void get_free_pages_test(void)
{
	char *buff;
	int order;

	pr_info("get_free_pages test\n");

	order = get_order(8192 * 10);
	buff = (char *)(uintptr_t)__get_free_pages(GFP_KERNEL, order);
	if (buff) {
		sprintf(buff, "__get_free_pages test ok [%d]\n", order);
		pr_info("%s", buff);
		free_pages((uintptr_t)buff, order);
	}
}

static int memtest_init(void)
{
	pr_info("Module Memory Test\n");

	kmalloc_test();
	vmalloc_test();
	get_free_pages_test();

	return 0;
}

static void memtest_exit(void)
{
	pr_info("Module Memory Test End\n");
}

module_init(memtest_init);
module_exit(memtest_exit);

MODULE_LICENSE("Dual BSD/GPL");

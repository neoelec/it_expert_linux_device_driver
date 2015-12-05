#include <linux/kernel.h>
#include <linux/module.h>

extern int func_var1;
extern int func_var2;
extern int func_sum(int var3);

static int calldev_init(void)
{
	func_var1 = 3;
	func_var2 = 4;

	pr_info("%d + %d + 5 = %d\n", func_var1, func_var2, func_sum(5));

	return 0;
}

static void calldev_exit(void)
{
}

module_init(calldev_init);
module_exit(calldev_exit);

MODULE_LICENSE("Dual BSD/GPL");

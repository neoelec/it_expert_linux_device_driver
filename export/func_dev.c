#include <linux/kernel.h>
#include <linux/module.h>

static int func_var1;
EXPORT_SYMBOL(func_var1);

static int func_var2;
EXPORT_SYMBOL(func_var2);

static int func_sum(int var3)
{
	pr_info("func_var1 = %d\n", func_var1);
	pr_info("func_var2 = %d\n", func_var2);
	pr_info("var3      = %d\n", var3);

	return func_var1 + func_var2 + var3;
}
EXPORT_SYMBOL(func_sum);

static int funcdev_init(void)
{
	return 0;
}

static void funcdev_exit(void)
{
}

module_init(funcdev_init);
module_exit(funcdev_exit);

MODULE_LICENSE("Dual BSD/GPL");

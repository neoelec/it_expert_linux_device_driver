#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mempool.h>

#define MIN_ELEMENT		4
#define TEST_ELEMENT		4

typedef struct {
	int number;
	char string[128];
} TMemElement;

static int elementcount;

static void *mempool_alloc_test(gfp_t gfp_mask, void *pool_data)
{
	TMemElement *data;

	pr_info("----> %s\n", __func__);

	data = kmalloc(sizeof(TMemElement), gfp_mask);
	if (data)
		data->number = elementcount++;

	return data;
}

static void mempool_free_test(void *element, void *pool_data)
{
	pr_info("----> call %s\n", __func__);

	if (element)
		kfree(element);
}

static int mempool_init(void)
{
	mempool_t *mp;
	TMemElement *element[TEST_ELEMENT];
	int lp;

	pr_info("Module MEMPOOL Test\n");

	memset(element, 0, sizeof(element));

	pr_info("call mempool_create\n");
	mp = mempool_create(MIN_ELEMENT, mempool_alloc_test,
			mempool_free_test, NULL);

	pr_info("mempool allocate\n");
	for (lp = 0; lp < ARRAY_SIZE(element); lp++) {
		element[lp] = mempool_alloc(mp, GFP_KERNEL);
		if (!element[lp])
			pr_warn("allocate_fail\n");
		else {
			sprintf(element[lp]->string, "alloc data %d\n",
					element[lp]->number);
			pr_info("%s", element[lp]->string);
		}
	}

	pr_info("mempool free\n");
	for (lp = 0; lp < ARRAY_SIZE(element); lp++)
		if (element[lp])
			mempool_free(element[lp], mp);

	pr_info("call mempool_destroy\n");
	mempool_destroy(mp);

	return 0;
}

static void mempool_exit(void)
{
	pr_info("Module MEMPOOL Test End\n");
}

module_init(mempool_init);
module_exit(mempool_exit);

MODULE_LICENSE("Dual BSD/GPL");

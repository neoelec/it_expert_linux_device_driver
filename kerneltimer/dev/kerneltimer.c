#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define KERNELTIMER_WRITE_ADDR		0x0378
#define TIME_STEP			(2 * HZ / 10)

typedef struct
{
	struct timer_list timer;
	unsigned long led;
} __attribute__((packed)) KERNEL_TIMER_MANAGER;

static KERNEL_TIMER_MANAGER *ptrmng;

static void kerneltimer_timeover(unsigned long arg);

static void kerneltimer_registertimer(KERNEL_TIMER_MANAGER *pdata,
		unsigned long timeover)
{
	init_timer(&(pdata->timer));
	pdata->timer.expires = get_jiffies_64() + timeover;
	pdata->timer.data = (unsigned long)pdata;
	pdata->timer.function = kerneltimer_timeover;
	add_timer(&(pdata->timer));
}

static void kerneltimer_timeover(unsigned long arg)
{
	KERNEL_TIMER_MANAGER *pdata = NULL;

	if (arg) {
		pdata = (KERNEL_TIMER_MANAGER *)arg;

		outb((u8)(pdata->led & 0xff), KERNELTIMER_WRITE_ADDR);

		pdata->led = ~(pdata->led);

		kerneltimer_registertimer(pdata, TIME_STEP);
	}
}

static int kerneltimer_init(void)
{
	ptrmng = kzalloc(sizeof(KERNEL_TIMER_MANAGER), GFP_KERNEL);
	if (!ptrmng)
		return -ENOMEM;

	ptrmng->led = 0;
	kerneltimer_registertimer(ptrmng, TIME_STEP);

	return 0;
}

static void kerneltimer_exit(void)
{
	if (ptrmng) {
		del_timer(&(ptrmng->timer));
		kfree(ptrmng);
	}

	outb(0x00, KERNELTIMER_WRITE_ADDR);
}

module_init(kerneltimer_init);
module_exit(kerneltimer_exit);

MODULE_LICENSE("Dual BSD/GPL");

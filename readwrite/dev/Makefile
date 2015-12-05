obj-m			:= rdwr_dev.o

KDIR			:= /lib/modules/$(shell uname -r)/build
PWD			:= $(shell pwd)

default:
	@$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

DEV_NODE		:= /dev/rdwrdev

insmod:
	@sudo mknod $(DEV_NODE) c 240 0
	@sudo insmod $(obj-m:.o=.ko)

rmmod:
	@sudo rmmod $(obj-m:.o=.ko)
	@sudo rm -f $(DEV_NODE)

clean:
	@$(RM) -f *.ko
	@$(RM) -f *.mod.*
	@$(RM) -f .*.cmd
	@$(RM) -f *.o
	@$(RM) -f modules.order  Module.symvers
	@$(RM) -rf .tmp_versions

obj-m			:= minor_dev.o

KDIR			:= /lib/modules/$(shell uname -r)/build
PWD			:= $(shell pwd)

default:
	@$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

DEV_WRITE_NODE		:= /dev/minor_write
DEV_READ_NODE		:= /dev/minor_read

insmod:
	@sudo mknod $(DEV_WRITE_NODE) c 240 1
	@sudo mknod $(DEV_READ_NODE) c 240 2
	@sudo insmod $(obj-m:.o=.ko)

rmmod:
	@sudo rmmod $(obj-m:.o=.ko)
	@sudo rm -f $(DEV_READ_NODE)
	@sude rm -f $(DEV_WRITE_NODE)

clean:
	@$(RM) -f *.ko
	@$(RM) -f *.mod.*
	@$(RM) -f .*.cmd
	@$(RM) -f *.o
	@$(RM) -f modules.order  Module.symvers
	@$(RM) -rf .tmp_versions

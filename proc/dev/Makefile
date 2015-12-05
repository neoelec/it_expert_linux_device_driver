obj-m			:= proc_dev.o

KDIR			:= /lib/modules/$(shell uname -r)/build
PWD			:= $(shell pwd)

default:
	@$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	@sudo insmod $(obj-m:.o=.ko)

rmmod:
	@sudo rmmod $(obj-m:.o=.ko)

clean:
	@$(RM) -f *.ko
	@$(RM) -f *.mod.*
	@$(RM) -f .*.cmd
	@$(RM) -f *.o
	@$(RM) -f modules.order  Module.symvers
	@$(RM) -rf .tmp_versions

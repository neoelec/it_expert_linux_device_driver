obj-m			:= func_dev.o
obj-m			+= call_dev.o

KDIR			:= /lib/modules/$(shell uname -r)/build
PWD			:= $(shell pwd)

default:
	@$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	@for mod in $(obj-m:.o=.ko); do sudo insmod $$mod; done

rmmod:
	@for mod in $(obj-m:.o=.ko); do sudo rmmod $$mod; done

clean:
	@$(RM) -f *.ko
	@$(RM) -f *.mod.*
	@$(RM) -f .*.cmd
	@$(RM) -f *.o
	@$(RM) -f modules.order  Module.symvers
	@$(RM) -rf .tmp_versions

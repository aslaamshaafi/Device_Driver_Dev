#$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
#$(MAKE) -C $(KDIR) M=$(PWD) modules--recently introduced in kernel
obj-m =first_module.o
KDIR= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean: 
	 rm -rf $(wildcard *.o *.ko *.mod.* .c* .t* test Module.symvers *.order *.markers)

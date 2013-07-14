module_name:=DS18B20

obj-m:=$(module_name).o

KERNELDIR ?=/home/izobs/opt/opt/EmbedSky/linux-2.6.30.4


PWD :=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install:
	cp -f /home/izobs/Linux_c/arm-$(module_name)/module/$(module_name).ko /home/izobs/linux/bin


clean:
	rm -rf *.o *~ *.ko *.mod.c *.order *.symvers .*.o.cmd .*.ko.cmd

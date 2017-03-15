#obj-m += example1.o
#obj-m += exsysfs1.o
obj-m += myvfs.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

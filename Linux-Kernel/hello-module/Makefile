#current path
PWD = $(shell pwd)

# kernel version
KVERS =$(shell uname -r)
#kernel dir
KERNDIR =/lib/modules/${KVERS}/build/

#Specify flags for module compilation
#EXTRA_CFLAGS =-g -o0

#kernel modules
obj-m += hello.o

# for multi source code file suce file1.c file2.c
# obj-m += modulename.o
# modulename-objs +=file1.o file2.o

build: kernel_modules

kernel_modules:
	make -C $(KERNDIR) M=$(PWD) modules

clean:
	make -C $(KERNDIR) M=$(PWD) clean
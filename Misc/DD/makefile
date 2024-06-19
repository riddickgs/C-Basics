#this is makefile for simple driver for kernel

obj-m+= hello_world.o driver.o
CROSS_COMPILE:= /usr/bin/arm-linux-gnueabi-gcc

ifdef ARCH
	KDIR = /home/linux/CodePractice/DD/build
else
	KDIR = /lib/modules/$(shell uname -r)/build
endif

all:
	make -C $(KDIR) M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean

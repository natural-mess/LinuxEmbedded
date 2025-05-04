# GPIO Subsystem LED driver for Raspberry Pi 4
KERNEL := /home/rocky/Workspace/yocto/poky/build/workspace/sources/linux-raspberrypi
TOOLCHAIN := /opt/poky/3.1.33/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-

EXTRA_CFLAGS = -Wall
obj-m := led.o

all:
    make ARCH=arm64 CROSS_COMPILE=$(TOOLCHAIN) -C $(KERNEL) M=$(shell pwd) modules

clean:
    make -C $(KERNEL) M=$(shell pwd) clean
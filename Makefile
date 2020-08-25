# SPDX-License-Identifier: GPL-2.0
# Copyright(c) 1999 - 2018 Intel Corporation.

#
# Makefile for the Intel(R) PRO/1000 ethernet driver
#

obj-m += e1000e-corruptor.o cachedisable.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

e1000e-corruptor-objs := 82571.o ich8lan.o 80003es2lan.o \
	       mac.o manage.o nvm.o phy.o \
	       param.o ethtool.o netdev.o ptp.o

reader:
	gcc read_mapping.c -o read_mapping

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

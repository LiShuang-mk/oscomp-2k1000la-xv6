#!/bin/bash 

RUNENV_PREFIX="/home/lishuang/qemu-loongarch/qemu0401"
KERNEL_PREFIX=`pwd`

cd $RUNENV_PREFIX

./bin/qemu-system-loongarch64 \
	-M ls2k \
	-serial stdio \
	-k ./share/qemu/keymaps/en-us \
	-kernel ${KERNEL_PREFIX}/build/kernel.elf \
	-serial vc \
	-m 1G \
	-device usb-kbd,bus=usb-bus.0 \
	-device usb-tablet,bus=usb-bus.0 \
	-device usb-storage,drive=udisk \
	-drive if=none,id=udisk,file=./tmp/disk \
	-net nic \
	-net user,net=10.0.2.0/24,tftp=/srv/tftp \
	-vnc :0 \
	-s -S \
	-hda ./sdcard.img
	# -hda ./tmp/2kfs.img \
	# -D ./tmp/qemu.log \
	# -D /dev/null \

	#-initrd ${KERNEL_PREFIX}/fs.img \	 
# init ram file system 
# echo $RUN_CMD
# $RUN_CMD
#!/bin/bash 

RUNENV_PREFIX="/home/lishuang/qemu-loongarch/qemu0401"
# RUNENV_PREFIX="/home/lishuang/qemu-test"
KERNEL_PREFIX=`pwd`

cd $RUNENV_PREFIX

./bin/qemu-system-loongarch64 \
	-M ls2k \
	-serial stdio \
	-k ./share/qemu/keymaps/en-us \
	-drive if=pflash,file=./2k1000/u-boot-with-spl.bin \
	-serial vc \
	-m 1024 \
	-device usb-kbd,bus=usb-bus.0 \
	-device usb-tablet,bus=usb-bus.0 \
	-device usb-storage,drive=udisk \
	-drive if=none,id=udisk,file=./tmp/disk \
	-net nic \
	-net user,net=10.0.2.0/24,tftp=/srv/tftp \
	-vnc :0 \
	-s \
	-hda ./2kfs.img \
	-hdb ./sdcard-loongarch.img

	# -hda ./sdcard.img
	
	# -D ./tmp/qemu.log \
	# -D /dev/null \




	#-initrd ${KERNEL_PREFIX}/fs.img \	 
# init ram file system 
# echo $RUN_CMD
# $RUN_CMD
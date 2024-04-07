#!/bin/bash 

RUNENV_PREFIX="/home/amaranta/Downloads/qemu-static-20240401/qemu"
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
	-net nic \
	-net user,net=127.0.0.1/8,tftp=/srv/tftp \
	-vnc :0 \
	-s -S 
	# -hda ./tmp/2kfs.img 
	# -D /dev/null \
	# -vnc :0 -D ./tmp/qemu.log

	#-initrd ${KERNEL_PREFIX}/fs.img \	 
# init ram file system 
# echo $RUN_CMD
# $RUN_CMD

<font face="Liberation Mono">

###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队

-------------------------------------------------------------

[`<= 回到目录`](../README.md)

# qemu-ls2k + gdb 调试方式

参考下列 shell 命令: 

```sh
#!/bin/bash 

# 通常情况下该文件应当放在项目的根目录下

RUNENV_PREFIX=your_qemu_path
KERNEL_PREFIX=`pwd`

cd $RUNENV_PREFIX

./bin/qemu-system-loongarch64 \
	-M ls2k \
	-serial stdio \
	-k ./share/qemu/keymaps/en-us \
	-kernel ${KERNEL_PREFIX}/build/kernel.elf \
	-serial vc \
	-m 1G \
	-net nic \
	-net user,net=10.0.2.0/24,tftp=/srv/tftp \
	-vnc :0 \
	-S -s \
	-hda your_2kfs_img \
	-hdb your_sdcard_img 

	
	# -D ./tmp/qemu.log \
	# -D /dev/null \

	# -device usb-kbd,bus=usb-bus.0 \
	# -device usb-tablet,bus=usb-bus.0 \
	# -device usb-storage,drive=udisk \
	# -drive if=none,id=udisk,file=./tmp/disk \



	#-initrd ${KERNEL_PREFIX}/fs.img \	 
# init ram file system 
# echo $RUN_CMD
# $RUN_CMD

```
#!/bin/bash 

if [ -f ./build/kernel.elf ]; then {
	loongarch64-linux-gnu-objdump -D -C ./build/kernel.elf > ./build/kernel.dis;
	loongarch64-linux-gnu-objdump -S -C ./build/kernel.elf > ./build/kernel.asm;
	echo "反汇编成功！";
}
else {
	echo "kernel.elf 不存在！";
}
fi 

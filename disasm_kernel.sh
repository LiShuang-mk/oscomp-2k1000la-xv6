#!/bin/bash 

if [ -f ./build/kernel.elf ]; then {
	loongarch64-linux-gnu-objdump -D ./build/kernel.elf > ./build/kernel.dis;
	echo "反汇编成功！";
}
else {
	echo "kernel.elf 不存在！";
}
fi 

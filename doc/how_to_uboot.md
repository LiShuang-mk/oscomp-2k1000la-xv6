<font face="Liberation Mono">

###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队

-------------------------------------------------------------

[`<= 回到目录`](../README.md)

# 如何使用uboot启动并调试内核

### I. 使用uboot启动

使用以下 ls2k_release 的启动方式

```sh
#!/bin/bash 

RUNENV_PREFIX="your_qemu_path"
KERNEL_PREFIX=`pwd`

cd $RUNENV_PREFIX

./bin/qemu-system-loongarch64 \
	-M ls2k \
	-k ./share/qemu/keymaps/en-us \
	-serial stdio \
	-serial vc \
	-drive if=pflash,file=./2k1000/u-boot-with-spl.bin \
	-m 1024 \
	-device usb-kbd,bus=usb-bus.0 \
	-device usb-tablet,bus=usb-bus.0 \
	-net nic \
	-net user,net=10.0.2.0/24,tftp=/srv/tftp \
	-vnc :0 \
	-s -S \
	-hda ./2kfs.img \
	-hdb ./sdcard-loongarch.img
```

注意 sdcar-loongarch.img 就是测例sdcard

在启动之前，我们需要将二进制内核 kernel.bin 放入 2kfs.img 中:

1. 主机安装 nbd 模块：

		sudo modprobe nbd

2. 挂载 2kfs.img 硬盘映像到nbd:

		sudo qemu-nbd -c /dev/nbd0 2kfs.img

3. 将硬盘的 ext4 分区挂载到主机文件系统上:

		sudo mount /dev/nbd0p1 mnt

4. 拷贝 kernel.bin 到 2kfs 中:

		sudo cp kernel.bin mnt/kernel.bin

5. 取消挂载:

		sudo umount mnt
		sudo qemu-nbd -d /dev/nbd0

可以将以上命令做成shell脚本:

```sh
RUNENV_PREFIX="your_qemu_path"
KERNEL_PREFIX="2k1000la_xv6_path"


cd $RUNENV_PREFIX

sudo modprobe nbd
sudo qemu-nbd -c /dev/nbd0 ./2kfs.img
sudo mount /dev/nbd0p1 ./mnt/
sudo cp ${KERNEL_PREFIX}/kernel.bin ./mnt/kernel.bin
sudo umount ./mnt/
sudo qemu-nbd -d /dev/nbd0
```

拷贝完后启动qemu，进入uboot界面

uboot 启动后及时按 c 进入控制台，或者按 m 进入菜单，菜单内有跳转到控制台的选项。

此时执行 `scsi reset`，检测SCSI设备

然后执行 `ext4load scsi 0 ${loadaddr} /kernel.bin` 可以加载内核到内存

使用 `md ${loadaddr}` 可以查看内核是否被正确加载

最后使用 `go ${loadaddr}` 跳转到内核执行


### II. 如何调试uboot启动的内核

vscode tasks.json 参考下面的任务

```json
{
			"label": "qemu ls2k uboot",
			"type": "shell",
			"command": "echo 'TaskInfo: QEMU Starting' && echo 'TaskInfo: Start Debug' && gnome-terminal -- ./ls2k_release.sh",
			"presentation": {
				"echo": true,
				"clear": true,
				"group": "qemu"
			},
			"isBackground": true,
			"problemMatcher": [
				{
					"pattern": {
						"regexp": ".*(错误)",
						"severity": 0
					},
					"background": {
						"activeOnStart": true,
						"beginsPattern": "^TaskInfo: QEMU Starting",
						"endsPattern": "^TaskInfo: Start Release"
					}
				},
				"$gcc"
			]
		}
```

vscode launch.json 参考下面的启动

```json
		{
			"name": "qemu-ls2k-start(uboot)",
			"type": "cppdbg",
			"request": "launch",
			"cwd": "${workspaceFolder}",
			"program": "./build/kernel.elf",
			"args": [],
			"stopAtEntry": false,
			"environment": [],
			"externalConsole": true,
			"MIMode": "gdb",
			"miDebuggerPath": "loongarch64-linux-gnu-gdb",
			"miDebuggerServerAddress": "localhost:1234",
			"preLaunchTask": "qemu ls2k uboot"
		}
```

此处虽然使用的是 kernel.bin，但调试的是 kernel.elf，实测有效，但是要保证两个文件是一致的，否则调试会出现错位。
<font face="Maple Mono SC NF">


###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队 

-------------------------------------------------------------

[`<= 回到目录`](../README.md)

# HSAI 参考文档

本文档将详细阐述 HSAI 模块中各文件内的类和函数用途，以及留给 HAL 和 kernel 应当实现的接口功能。

## 模块概览

- [hsai_defs.h](#hsai_defsh)
- [hsai_global.hh]()
- [hsai_log.hh]()
- [process_interface.hh]()
- [syscall_interface.hh]()
- [timer_interface.hh]()
- [virtual_cpu.hh]()
- [mem/]()
	- [page.hh]()
	- [page_table.hh]()
	- [virtual_memory.hh]()
- [smp/]()
	- [spin_lock.hh]()
- [uart/]()
	- [uart_ns16550.hh]()
	- [virtual_uart.hh]()
- [intr/]()
	- [virtual_interrrupt_manager.hh]()

## 正文

### hsai_defs.h

#### 宏 `__hsai_kernel`

这个宏是一个单纯的标签，用于提示内核应当实现其逻辑。

#### 宏 `__hsai_hal`

这个宏是一个单纯的标签，用于提示硬件抽象层应当实现其逻辑。

#### 宏 `NUMCPU`

这个宏用于配置架构CPU核心数。

### hsai_global.hh

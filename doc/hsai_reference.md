`<font face="Maple Mono SC NF">`

###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队

---

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
  - [virtual_page_table.hh]()
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

#### 宏 `DEV_TBL_LEN`

这个宏用于配置设备表最大长度。

#### 宏 `DEV_FIRST_NOT_RSV`

这个宏用于配置设备表中第一个非保留设备。

#### 宏 `DEV_STDIN_NUM`

标准输入流设备号。

#### 宏 `DEV_STDOUT_NUM`

标准输出流设备号。

#### 宏 `DEV_STDERR_NUM`

标准错误流设备号。

### hsai_global.hh

#### 命名空间 `hsai`

##### 函数 `hardware_abstract_init`

硬件抽象层初始化函数，用于初始化硬件抽象层。

##### 函数 `hardware_secondary_init`

硬件抽象层初始化函数，用于初始化硬件抽象层的第二阶段。

##### 函数 `hsai_internal_init`

HSAI 内部初始化函数，用于初始化 HSAI 模块。

#### 类 `VirtualCpu`

虚拟 CPU 类，用于描述一个虚拟 CPU。

##### 数组 `k_cpus[ NUMCPU ]`

##### 函数 `get_cpu`

#### 类 `VirtualMemory`

虚拟内存类，用于描述一个虚拟内存。

##### 指针 `k_mem`

#### 类 `VirtualInterruptManager`

虚拟中断管理器类，用于描述一个虚拟中断管理器。

##### 指针 `k_im`

#### 类 `DeviceManager`

设备管理器类，用于描述一个设备管理器。

##### 对象 `k_devm`

#### 类 `ConsoleStdin`

标准输入流类，用于描述一个标准输入流。

##### 对象 `k_stdin`

#### 类 `ConsoleStdout`

标准输出流类，用于描述一个标准输出流。

##### 对象 `k_stdout`

#### 类 `ConsoleStderr`

标准错误流类，用于描述一个标准错误流。

##### 对象 `k_stderr`

### hsai_log.hh

#### 命名空间  `hsai`

##### 枚举 `HsaiLogLevel`

日志级别枚举。包括 log_trace、log_info、log_warn、log_error 和 log_panic。

##### 函数指针 `p_hsai_logout`

用于日志输出，接受日志级别、文件名、行号和日志信息等参数。

##### 函数指针 `p_hsai_assert`

用于断言，接受文件名、行号、表达式和详细信息等参数。

##### 函数指针 `p_hsai_printf`

用于格式化输出，接受格式化字符串和参数。

#### 宏 `hsai_trace`、`hsai_info、hsai_warn、hsai_error、hsai_panic`

接受两个参数：trace_info 和 args。trace_info 是一个字符串，用于描述日志信息的主要内容，而 args 则是可变参数，用于传递额外的日志信息。

#### 宏 `hsai_assert`

接受三个参数：expr、detail 和 args。expr 是一个表达式，用于判断是否发生错误，detail 是一个字符串，用于描述错误的详细信息，而 args 则是可变参数，用于传递额外的日志信息。

#### 宏 `hsai_printf`

接受两个参数：fmt 和 args。fmt 是一个格式化字符串，用于描述输出的格式，而 args 则是可变参数，用于传递额外的日志信息。

#### 宏 `COLOR_PRINT`、`RED_COLOR_PINRT`、`GREEN_COLOR_PRINT`、`YELLOW_COLOR_PRINT`、`BLUE_COLOR_PRINT`、`CYAN_COLOR_PINRT`、`CLEAR_COLOR_PRINT`

RED_COLOR_PINRT：定义为 "\033[31m"，用于将文本颜色设置为红色。
GREEN_COLOR_PRINT：定义为 "\033[32m"，用于将文本颜色设置为绿色。
YELLOW_COLOR_PRINT：定义为 "\033[33m"，用于将文本颜色设置为黄色。
BLUE_COLOR_PRINT：定义为 "\033[34m"，用于将文本颜色设置为蓝色。
CYAN_COLOR_PINRT：定义为 "\033[36m"，用于将文本颜色设置为青色。
CLEAR_COLOR_PRINT：定义为 "\033[0m"，用于重置文本颜色，恢复到默认颜色。

### process_interface.hh

#### 命名空间 `process_interface`

#### 类 `VirtualPageTable`、`SpinLock`

这两个类用于描述虚拟页表和自旋锁。

#### 常量 `proc_pool_size`

进程池大小。

#### 函数 `get_cur_proc`

获取当前进程。

#### 函数 `get_trap_frame_from_proc`

从进程中获取陷阱帧。

#### 函数 `get_trap_frame_vir_addr`

获取陷阱帧虚拟地址。

#### 函数 `get_pid`

获取进程 ID。

#### 函数 `exit_proc`

退出进程。

#### 函数 `proc_is_killed`

判断进程是否被杀死。

#### 函数 `proc_is_running`

判断进程是否正在运行。

#### 函数 `sched_proc`

调度进程。

#### 函数 `get_kstack_from_proc`

从进程中获取内核栈。

#### 函数 `get_kstack_size`

获取内核栈大小。

#### 函数 `get_pgd_addr`

获取页目录地址。

#### 函数 `get_pt_from_proc`

从进程中获取页表。

#### 函数 `sleep_at`

在某个通道上休眠，并获取自旋锁。

#### 函数 `wakeup_at`

唤醒某个通道上的进程。

#### 函数 `user_proc_init`

用户进程初始化。

#### 函数 `set_trap_frame_return_value`

设置陷阱帧返回值。

#### 函数 `set_trap_frame_entry`

设置陷阱帧入口地址。

#### 函数 `set_trap_frame_user_sp`

设置陷阱帧用户栈指针。

#### 函数 `set_trap_frame_arg`

设置陷阱帧的参数。

#### 函数 `copy_trap_frame`

复制陷阱帧。

#### 函数 `get_arg_from_trap_frame`

从陷阱帧中获取参数。

#### 函数 `user_trap_return`

用户陷阱返回。

#### 常量 `context_size`

上下文大小。

#### 函数 `set_context_entry`

设置上下文入口地址。

#### 函数 `set_context_sp`

设置上下文栈指针。

#### 函数 `get_context_address`

获取上下文地址。

### syscall_interface.hh

#### 命名空间 `hsai`

##### 函数 `kernel_syscall`

内核系统调用函数，用于执行内核系统调用。

##### 函数 `get_syscall_max_num`

获取系统调用最大编号。

### timer_interface.hh

#### 命名空间 `hsai`

##### 函数 `handle_tick_intr`

处理时钟中断。

##### 函数 `get_ticks`

获取时钟计数。

##### 函数 `get_main_frequence`

获取主频。

##### 函数 `cycles_per_tick`

获取每个时钟周期的时钟数。

##### 函数 `get_hw_time_stamp`

获取硬件时间戳。

##### 函数 `time_stamp_to_usec`

时间戳转换为微秒。

##### 函数 `usec_to_time_stamp`

微秒转换为时间戳。

### virtual_cpu.hh

#### 命名空间 `pm`

##### 类 `Pcb`

#### 命名空间 `hsai`

##### 类 `VirtualCpu`

类的成员变量：

- _num_off：记录中断关闭的次数。
- _int_ena：记录在调用 push-off() 之前的中断使能状态。
- _cur_proc：指向当前进程的指针，类型为 pm::Pcb*。
- _context：指向上下文的指针。

类的成员函数：

- `VirtualCpu()`：构造函数。
- `static int register_cpu( VirtualCpu * p_cpu, int cpu_id )`：注册 CPU。
- `virtual uint get_cpu_id()`：获取 CPU ID。
- `virtual int is_interruptible()`：判断是否可中断。
- `virtual void _interrupt_on()`：打开中断。
- `virtual void _interrupt_off()`：关闭中断。
- `int get_num_off()`：获取中断关闭次数。
- `int get_int_ena()`：获取中断使能状态。
- `pm::Pcb * get_cur_proc()`：获取当前进程。
- `void * get_context()`：获取上下文。
- `void set_cur_proc( pm::Pcb * proc )`：设置当前进程。
- `void set_int_ena( int int_ena )`：设置中断使能状态。
- `void push_interrupt_off()`
- `void pop_intterupt_off()`

### mem/page.hh

#### 常量 `PG_SIZE`、`PN_MASK`、`PT_LEVEL`

`PG_SIZE`：页大小，默认为 4KB。
`PN_MASK`：页号掩码，默认为 0x1FF。
`PT_LEVEL`：页表级别，默认为 4。

#### 命名空间 `hsai`

##### 类 `Pte`

`Pte` 类用于描述页表项。

- `_data_addr`：页表项的指针。
- `Pte()`：构造函数。
- `Pte( pte_t *addr )`：构造函数。
- `set_addr( pte_t *addr )`：设置页表项的指针。
- `is_null()`：判断页表项是否为空。
- `is_valid()`：判断页表项是否有效。
- `is_dirty()`：判断页表项是否脏。
- `is_present()`：判断页表项是否存在。
- `is_writable()`：判断页表项是否可写。
- `is_readable()`：判断页表项是否可读。
- `is_executable()`：判断页表项是否可执行。
- `is_dir_page()`：判断页表项是否目录页。
- `is_super_plv()`：判断页表项是否超级页表。
- `is_user_plv()`：判断页表项是否用户页表。
- `set_user_plv()`：设置页表项为用户页表。
- `set_super_plv()`：设置页表项为超级页表。
- `to_pa()`：获取物理地址。
- `get_flags()`：获取页表项标志。
- `map_code_page_flags()`：获取代码页标志。
- `map_data_page_flags()`：获取数据页标志。
- `map_dir_page_flags()`：获取目录页标志。
- `super_plv_flag()`：获取超级页表标志。
- `user_plv_flag()`：获取用户页表标志。
- `valid_flag()`：获取有效标志。
- `clear_data()`：清空页表项。
- `get_data()`：获取页表项数据。
- `set_data( uint64 data )`：设置页表项数据。

##### 常量 `page_size`、`page_size_shift`、`page_number_mask`、`page_number_mask_width`

`page_size`：页大小。
`page_size_shift`：页大小的位移。
`page_number_mask`：页号掩码。
`page_number_mask_width`：页号掩码的宽度。

##### 函数 `page_round_up`、`page_round_down`、`is_page_align`

`page_round_up( ulong addr )`：向上对齐页。
`page_round_down( ulong addr )`：向下对齐页。
`is_page_align( uint64 addr )`：判断地址是否对齐。

##### 常量 `pgd_mask`、`pud_mask`、`pmd_mask`、`pt_mask`

`pgd_mask`：页目录掩码。
`pud_mask`：页上级目录掩码。
`pmd_mask`：页中间目录掩码。
`pt_mask`：页表掩码。

##### 函数 `lowest_1_bit`、`pgd_mask_shift`、`pud_mask_shift`、`pmd_mask_shift`、`pt_mask_shift`、`pgd_num`、`pud_num`、`pmd_num`、`pt_num`

`lowest_1_bit( ulong i )`：获取最低位 1 的位置。
`pgd_mask_shift`：页目录掩码的位移。
`pud_mask_shift`：页上级目录掩码的位移。
`pmd_mask_shift`：页中间目录掩码的位移。
`pt_mask_shift`：页表掩码的位移。
`pgd_num( ulong va )`：获取页目录号。
`pud_num( ulong va )`：获取页上级目录号。
`pmd_num( ulong va )`：获取页中间目录号。
`pt_num( ulong va )`：获取页表号。

### mem/virtual_page_table.hh

#### 命名空间 `hsai`

##### 类 `VirtualPageTable`

类的成员变量：

- `_pt_base`：页表基址。
- `VirtualPageTable()`：构造函数。
- `virtual Pte walk( ulong va, bool alloc ) = 0`：查找页表项。
- `virtual ulong walk_addr( ulong va ) = 0`：查找页表地址。

### mem/virtual_memory.hh

#### 命名空间 `hsai`

##### 类 `VirtualMemory`

类的成员函数：

- `virtual ulong mem_start() = 0`：获取内存起始地址。
- `virtual ulong mem_size() = 0`：获取内存大小。
- `virtual ulong to_vir( ulong addr ) = 0`：转换为虚拟地址。
- `virtual ulong to_phy( ulong addr ) = 0`：转换为物理地址。
- `virtual ulong to_io( ulong addr ) = 0`：转换为 I/O 地址。
- `virtual ulong to_dma( ulong addr ) = 0`：转换为 DMA 地址。
- `virtual void config_pt( ulong pt_addr ) = 0`：配置全局页表。
- `static int register_memory( VirtualMemory * mem )`：注册内存。

### smp/spin_lock.hh

#### 命名空间 `hsai`

##### 类 `SpinLock`

类的成员变量：

- `_name`：用于调试的名称。
- `_locked`：用于记录锁的状态。
- `SpinLock()`：构造函数。
- `void init( const char *name )`：初始化自旋锁。
- `void acquire()`：请求自旋锁。
- `void release()`：释放自旋锁。
- `bool is_held()`：判断自旋锁是否被持有。

### uart/uart_ns16550.hh

#### 命名空间 `hsai`

#### 类 `UartNs16550`

类的成员变量：

- `_buf_size`：缓冲区大小，默认为 64。
- `_lock`：自旋锁。
- `_reg_base`：寄存器基址。
- `_buf`：缓冲区。
- `_wr_idx`：写索引。
- `_rd_idx`：读索引。
- `UartNs16550()`：构造函数。
- `UartNs16550( void * reg_base )`：构造函数。
- `void init()`：初始化。
- `int put_char_sync( u8 c )`：同步发送字符。
- `int put_char( u8 c )`：发送字符。
- `int get_char_sync( u8 * c )`：同步接收字符。
- `int get_char( u8 * c )`：接收字符。
- `int handle_intr()`：处理中断。
- `void _start()`：开始。

枚举 `RegOffset`：

- `RHR`：接收保持寄存器。
- `THR`：发送保持寄存器。
- `IER`：中断使能寄存器。
- `FCR`：FIFO 控制寄存器。
- `ISR`：中断状态寄存器。
- `LCR`：线路控制寄存器。
- `MCR`：调制解调器控制寄存器。
- `LSR`：线路状态寄存器。
- `MSR`：调制解调器状态寄存器。
- `SPR`：保留寄存器。
- `DLL`：分频锁，最低有效字节。
- `DLM`：分频锁，最高有效字节。
- `PSD`：预分频器分频。

结构体 `regIER`：

- `data_ready`：数据准备。
- `thr_empty`：发送保持器空。
- `lsr_int`：线路状态中断。
- `modem_int`：调制解调器中断。
- `_rev`：保留。
- `dma_rx`：DMA 接收。
- `dma_tx`：DMA 发送。

结构体 `regFCR`：

- `enable`：使能。
- `rx_fifo_reset`：接收 FIFO 复位。
- `tx_fifo_reset`：发送 FIFO 复位。
- `dma_mode`：DMA 模式。
- `dma_end_enable`：DMA 结束使能。
- `_rev`：保留。
- `rx_fifo_level`：接收 FIFO 级别。

结构体 `regISR`：

- `intr_status`：中断状态。
- `intr_code`：中断代码。
- `dma_rx_end`：DMA 接收结束。
- `dma_tx_end`：DMA 发送结束。
- `fifo_enabled`：FIFO 使能。

结构体 `regLCR`：

- `WordLen`：字长。
- `word_length`：字长。
- `stop_bits`：停止位。
- `parity_enbale`：奇偶校验使能。
- `even_parity`：偶校验。
- `force_parity`：强制校验。
- `set_break`：设置中断。
- `divisor_access`：分频器访问。

结构体 `regMCR`：

- `dtr`：数据终端就绪。
- `rts`：请求发送。
- `out1`：输出 1。
- `out2`：输出 2。
- `loop_back`：环回。
- `_rev`：保留。

结构体 `regLSR`：

- `data_ready`：数据准备。
- `overrun_error`：溢出错误。
- `parity_error`：奇偶校验错误。
- `framing_error`：帧错误。
- `break_intr`：中断。
- `thr_empty`：发送保持器空。
- `transmit_empty`：发送空。
- `fifo_data_error`：FIFO 数据错误。

结构体 `regMSR`：

- `delta_cts`：CTS 变化。
- `delta_dsr`：DSR 变化。
- `trail_edge_ri`：RI 边沿。
- `delta_cd`：CD 变化。
- `cts`
- `dst`
- `ri`
- `cd`

结构体 `regPSD`：

- `precaler_division`：预分频器分频。
- `_rev`：保留。

`_write_reg()`：写寄存器。
`_read_reg()`：读寄存器。

### intr/virtual_interrrupt_manager.hh

#### 类 `VirtualInterruptManager`

类的成员函数：

- `virtual int handle_dev_intr() = 0`：处理设备中断。
- `static int register_interrupt_manager( VirtualInterruptManager * im )`：注册中断管理器。
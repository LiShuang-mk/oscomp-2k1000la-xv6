//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include <mem/page.hh>

// virtual memory layout 

namespace mm
{
	enum vml : uint64
	{
		vm_page_cnt_shift = 24,
		vm_user_start = 0x0,
		vm_user_end = vm_user_start + ( hsai::page_size << vm_page_cnt_shift ),		// 64 GiB
		vm_trap_frame = vm_user_end - hsai::page_size,								// 用户空间最后一个页面是 trap frame
		vm_ustack_end = vm_trap_frame - hsai::page_size,							// 用户栈结束于与trap frame相隔一个页面的地址

		vm_kernel_start = 0x3UL << 36,												// kernel address is 0x3*_****_****
		vm_kernel_end = vm_kernel_start + ( hsai::page_size << vm_page_cnt_shift ),	// 64 GiB

		// vm_kernel_heap_start = vm_kernel_start >> 1,
		// vm_kernel_heap_start = _1M * 8,
		// vm_kernel_heap_start = vm_end >> 1,							// 64 MiB
		// vm_kernel_heap_size = _1M * 64,								// 配置可用于堆内存的最大大小 ---- 已弃用
	};
	static_assert( vml::vm_user_end == 64 * _1G );
}
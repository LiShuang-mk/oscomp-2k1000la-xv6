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
		vm_page_cnt_shift = 20,
		vm_user_start = 0x0,
		vm_user_end = vm_user_start + ( hsai::page_size << vm_page_cnt_shift ),		// 4 GiB

		vm_kernel_start = 0xFFUL << 32,												// kernel address is 0xFF_****_****
		vm_kernel_end = vm_kernel_start + ( hsai::page_size << vm_page_cnt_shift ),	// 4 GiB
		vm_trap_frame = vm_kernel_end - hsai::page_size,							// 内核空间最后一个页面是 trap frame

		// vm_kernel_heap_start = vm_kernel_start >> 1,
		// vm_kernel_heap_start = _1M * 8,
		// vm_kernel_heap_start = vm_end >> 1,							// 64 MiB
		vm_kernel_heap_size = _1M * 64,									// 配置可用于堆内存的最大大小
	};
	static_assert( vml::vm_user_end == 4 * _1G );
}
//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "page.hh"

// virtual memory layout 

namespace mm
{
	enum vml : uint64
	{
		vm_page_cnt_shift = 14,
		vm_start = 0x0,
		vm_end = ( 0x1UL << pg_size_shift ) << vm_page_cnt_shift,	// 64 MiB

		vm_kernel_start = vm_end >> 1,
		vm_trap_frame = vm_end - pg_size,

		// vm_kernel_heap_start = vm_kernel_start >> 1,
		vm_kernel_heap_start = _1M * 8,
		vm_kernel_heap_size = _1M * 16,
	};
	static_assert( vml::vm_end == _1M * 64 );
}
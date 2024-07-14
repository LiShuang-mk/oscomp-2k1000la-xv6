//
// Created by Li Shuang ( pseudonym ) on 2024-07-10 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include <kernel/types.hh>

namespace hsai
{
	class VirtualMemory __hsai_hal
	{
	public:
		virtual ulong mem_start() = 0;
		virtual ulong mem_size() = 0;

		// to virtual address 
		virtual ulong to_vir( ulong addr ) = 0;

		// to physical address
		virtual ulong to_phy( ulong addr ) = 0;

		// to the address used to access I/O
		virtual ulong to_io( ulong addr ) = 0;

		// to the adress for DMA to access memory
		virtual ulong to_dma( ulong addr ) = 0;

		// configure global page table
		virtual void config_pt( ulong pt_addr ) = 0;

	public:
		static int register_memory( VirtualMemory * mem );
	};


} // namespace hsai

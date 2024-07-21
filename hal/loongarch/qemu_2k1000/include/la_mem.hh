//
// Created by Li Shuang ( pseudonym ) on 2024-07-10
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <mem/virtual_memory.hh>

namespace loongarch
{
	namespace qemu2k1000
	{
		class Memory : public hsai::VirtualMemory
		{
		public:
			static void memory_init();

		public:
			virtual ulong mem_start() override;
			virtual ulong mem_size() override;

			// to virtual address 
			virtual ulong to_vir( ulong addr ) override;

			// to physical address
			virtual ulong to_phy( ulong addr ) override;

			// to the address used to access I/O
			virtual ulong to_io( ulong addr ) override;

			// to the adress for DMA to access memory
			virtual ulong to_dma( ulong addr ) override;

			// configure global page table
			virtual void config_pt( ulong pt_addr ) override;

		};


	} // namespace qemu_2k1000


} // namespace loongarch

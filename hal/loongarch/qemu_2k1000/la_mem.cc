//
// Created by Li Shuang ( pseudonym ) on 2024-07-10 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "loongarch.hh"
#include "qemu_2k1000.hh"
#include "la_mem.hh"
#include "la_cpu.hh"
#include "tlb_manager.hh"

#include <hsai_global.hh>
#include <hsai_log.hh>
#include <mem/page.hh>

extern "C" {
	extern ulong xn6_end;
}

namespace loongarch
{
	namespace qemu2k1000
	{
		static Memory k_2k1000_mem;

		void Memory::memory_init()
		{
			register_memory( &k_2k1000_mem );

			// configure page table mapping

			Cpu * cpu = ( Cpu * ) hsai::get_cpu();

			k_tlbm.init( "tlb manager" );

			hsai_info( "pgdl : %p", cpu->read_csr( csr::pgdl ) );
			hsai_info( "pgdh : %p", cpu->read_csr( csr::pgdh ) );

			cpu->write_csr(
				csr::pwcl,
				( hsai::pt_mask_shift << csr::pwcl_pt_base_s ) |
				( hsai::page_number_mask_width << csr::pwcl_pt_width_s ) |
				( hsai::pmd_mask_shift << csr::pwcl_dir1_base_s ) |
				( hsai::page_number_mask_width << csr::pwcl_dir1_width_s ) |
				( hsai::pud_mask_shift << csr::pwcl_dir2_base_s ) |
				( hsai::page_number_mask_width << csr::pwcl_dir2_width_s ) |
				( csr::pwcl_pte_width_64bit << csr::pwcl_pte_width_s )
			);

			cpu->write_csr(
				csr::pwch,
				( hsai::pgd_mask_shift << csr::pwch_dir3_base_s ) |
				( hsai::page_number_mask_width << csr::pwch_dir3_width_s ) |
				( 0 << csr::pwch_dir4_base_s ) |
				( 0 << csr::pwch_dir4_width_s )
			);

			// config IO DMA accessing memory window 

			*( volatile uint64 * ) loongarch::qemu2k1000::XbarWin::xwin4_base0 =
				loongarch::qemu2k1000::iodma_win_base;
			*( volatile uint64 * ) loongarch::qemu2k1000::XbarWin::xwin4_mask0 =
				loongarch::qemu2k1000::iodma_win_mask;
			*( volatile uint64 * ) loongarch::qemu2k1000::XbarWin::xwin4_mmap0 =
				( loongarch::qemu2k1000::XbarWinMmap::xbar_win_cac_m ) |
				( loongarch::qemu2k1000::XbarWinMmap::xbar_win_ena_m ) |
				( loongarch::qemu2k1000::XbarWinMmap::xbar_win_num_m );

			// config secondary exchange window 

			*( volatile uint64 * ) loongarch::qemu2k1000::SecondWin::pci_win0_base =
				loongarch::qemu2k1000::iodma_win_base;
			*( volatile uint64* ) loongarch::qemu2k1000::SecondWin::pci_win0_mask =
				loongarch::qemu2k1000::iodma_win_mask;
			*( volatile uint64* ) loongarch::qemu2k1000::SecondWin::pci_win0_mmap =
				( loongarch::qemu2k1000::sec_win_route_ddr << loongarch::qemu2k1000::sec_win_rt_s ) |
				( loongarch::qemu2k1000::sec_win_br_m ) |
				( loongarch::qemu2k1000::sec_win_en_m );
		}

		ulong Memory::mem_start()
		{
			ulong end_addr = ( ulong ) &xn6_end;
			end_addr += _1M - 1;
			end_addr &= ~( _1M - 1 );
			return end_addr;
		}
		ulong Memory::mem_size()
		{
			ulong mem_end_addr = ( 0x90000000 | win_0 ) + _1G;
			return mem_end_addr - mem_start();
			// return memory::mem_size;
		}

		ulong Memory::to_vir( ulong addr )
		{
			return to_phy( addr ) | dmwin::win_0;
		}

		ulong Memory::to_phy( ulong addr )
		{
			return addr & ~dmwin_mask;
		}

		ulong Memory::to_io( ulong addr )
		{
			return to_phy( addr ) | dmwin::win_1;
		}

		ulong Memory::to_dma( ulong addr )
		{
			return to_phy( addr ) | iodma_win_base;
		}

		void Memory::config_pt( ulong pt_addr )
		{
			Cpu * cpu = ( Cpu * ) hsai::get_cpu();

			cpu->write_csr( csr::pgdl, to_phy( pt_addr ) );
			cpu->write_csr( csr::pgdh, to_phy( pt_addr ) );
		}

	} // namespace qemu2k1000

} // namespace loongarch

//
// Created by Li shuang ( pseudonym ) on 2024-04-09 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/pci_driver.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/sata/sata_ls2k.hh"
#include "hal/sata/hba_port.hh"
#include "mm/page.hh"
#include "mm/physical_memory_manager.hh"
#include "klib/common.hh"

namespace dev
{
	namespace pci
	{
		PciDriver k_pci_driver;

		void PciDriver::init( const char *lock_name )
		{
			_lock.init( lock_name );

			// config IO DMA accessing memory window 

			*( volatile uint64 * ) loongarch::qemuls2k::XbarWin::xwin4_base0 =
				loongarch::qemuls2k::iodma_win_base;
			*( volatile uint64 * ) loongarch::qemuls2k::XbarWin::xwin4_mask0 =
				loongarch::qemuls2k::iodma_win_mask;
			*( volatile uint64 * ) loongarch::qemuls2k::XbarWin::xwin4_mmap0 =
				( loongarch::qemuls2k::XbarWinMmap::xbar_win_cac_m ) |
				( loongarch::qemuls2k::XbarWinMmap::xbar_win_ena_m ) |
				( loongarch::qemuls2k::XbarWinMmap::xbar_win_num_m );

			// config secondary exchange window 

			*( volatile uint64 * ) loongarch::qemuls2k::SecondWin::pci_win0_base =
				loongarch::qemuls2k::iodma_win_base;
			*( volatile uint64* ) loongarch::qemuls2k::SecondWin::pci_win0_mask =
				loongarch::qemuls2k::iodma_win_mask;
			*( volatile uint64* ) loongarch::qemuls2k::SecondWin::pci_win0_mmap =
				( loongarch::qemuls2k::sec_win_route_ddr << loongarch::qemuls2k::sec_win_rt_s ) |
				( loongarch::qemuls2k::sec_win_br_m ) |
				( loongarch::qemuls2k::sec_win_en_m );

			// distribute command list and FIS buffer space 

			uint page_cnt, port;
			uint64 addr_base;

			// command list buffer space 
			page_cnt = ata::sata::max_port_num / ( mm::pg_size / ata::sata::hba_cmd_lst_len );
			port = 0;
			for ( uint i = 0; i < page_cnt; i++ )
			{
				addr_base = ( uint64 ) mm::k_pmm.alloc_page();
				if ( addr_base == 0x0 )
				{
					log_trace( "alloc port %d command list space: no mem", port );
					log_panic( "no mem" );
				}
				mm::k_pmm.clear_page( ( void* ) addr_base );
				addr_base &= ~loongarch::qemuls2k::dmwin_mask;
				addr_base |= loongarch::qemuls2k::iodma_win_base;
				for (
					uint64 addr = ( uint64 ) addr_base;
					addr < ( uint64 ) addr_base + mm::pg_size;
					addr += ata::sata::hba_cmd_lst_len )
				{
					ata::sata::k_sata_driver.set_port_cl( port, ( void* ) addr );
					++port;
				}
			}

			// FIS buffer space 
			page_cnt = ata::sata::max_port_num / ( mm::pg_size / ata::sata::hba_rec_fis_len );
			port = 0;
			for ( uint i = 0; i < page_cnt; i++ )
			{
				addr_base = ( uint64 ) mm::k_pmm.alloc_page();
				if ( addr_base == 0x0 )
				{
					log_trace( "alloc port %d receive FIS space: no mem", port );
					log_panic( "no mem" );
				}
				mm::k_pmm.clear_page( ( void* ) addr_base );
				addr_base &= ~loongarch::qemuls2k::dmwin_mask;
				addr_base |= loongarch::qemuls2k::iodma_win_base;
				for (
					uint64 addr = ( uint64 ) addr_base;
					addr < ( uint64 ) addr_base + mm::pg_size;
					addr += ata::sata::hba_rec_fis_len )
				{
					ata::sata::k_sata_driver.set_port_fb( port, ( void* ) addr );
					++port;
				}
			}

			log__info( "debug print clb and fb" );
			ata::sata::k_sata_driver.debug_print_cmd_lst_base();
			ata::sata::k_sata_driver.debug_print_rec_fis_base();

			// ata::sata::k_sata_driver.init( "sata driver", 0, 0 );
			ata::sata::k_sata_driver.simple_init();
		}

	} // namespace pci

} // namespace dev

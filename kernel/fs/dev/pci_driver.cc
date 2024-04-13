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
				( loongarch::qemuls2k::XbarWinMmap::xbar_win_ena_m ) |
				( loongarch::qemuls2k::XbarWinMmap::xbar_win_num_m );

			// distribute command list and FIS buffer space 
			
			uint64 clb_addr = ( uint64 ) mm::k_pmm.alloc_page();
			if ( clb_addr == 0x0 )
				log_panic( "pci driver: no memory" );
			uint64 fb_addr = ( uint64 ) mm::k_pmm.alloc_page();
			if ( fb_addr == 0x0 )
				log_panic( "pci driver: no memory" );

			// if ( ( ( uint64 ) clb_addr & ( 0xFUL << 60 ) ) != loongarch::qemuls2k::dmwin::win_0 )
			// 	log_panic( "addr not win0" );

			clb_addr &= ~loongarch::qemuls2k::dmwin_mask;
			clb_addr |= loongarch::qemuls2k::iodma_win_base;
			fb_addr &= ~loongarch::qemuls2k::dmwin_mask;
			fb_addr |= loongarch::qemuls2k::iodma_win_base;

			printf( "clb addr = %p\n", clb_addr );
			printf( "fb  addr = %p\n", fb_addr );

			ata::sata::k_sata_driver.init( "sata driver", ( void * ) clb_addr, ( void * ) fb_addr );
		}

	} // namespace pci

} // namespace dev

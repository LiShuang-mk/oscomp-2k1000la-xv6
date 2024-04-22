//
// Created by Li shuang ( pseudonym ) on 2024-04-09 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/pci_driver.hh"
#include "fs/dev/sata_driver.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/pci/pci_cfg_header.hh"
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

			sata_init();
		}

		void PciDriver::sata_init()
		{
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
					sata::k_sata_driver.set_port_clb( port, ( void* ) addr );
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
					sata::k_sata_driver.set_port_fb( port, ( void* ) addr );
					++port;
				}
			}

			// 读取 PCI 配置头
			dev::pci::PciCfgHeader *pciHead = ( dev::pci::PciCfgHeader * ) loongarch::qemuls2k::PciCfgDevAddr::pci_cfg_sata;
			if ( pciHead->vendor_id != 0x0014 )
				log_panic( "PCI SATA配置头不合规 - vendor id is not 0x0014" );

			// 从配置头获取 HBA Memory Registers base address 
			uint64 sata_mem_base = pciHead->base_address[ 0 ];
			sata_mem_base |= ( uint64 ) pciHead->base_address[ 1 ] << 32;
			sata_mem_base |= loongarch::qemuls2k::dmwin::win_1;				// 使用非缓存窗口 
			log_trace( "SATA 内部寄存器基地址: %p\n", sata_mem_base );
			sata::k_sata_driver.set_hba_mem_reg( ( void* ) sata_mem_base );

			log__info( "debug print clb and fb" );
			// sata::k_sata_driver.debug_print_cmd_lst_base();
			// sata::k_sata_driver.debug_print_rec_fis_base();

			// 设置SATA端口数量，这里硬设置为3
			sata::k_sata_driver.set_port_num( 3 );

			sata::k_sata_driver.init( "sata" );
		}

	} // namespace pci

} // namespace dev
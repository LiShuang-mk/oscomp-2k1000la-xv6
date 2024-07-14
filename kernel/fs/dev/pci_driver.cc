//
// Created by Li shuang ( pseudonym ) on 2024-04-09 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/pci_driver.hh"
#include "fs/dev/ahci_controller.hh"
#include "fs/dev/sata_driver.hh"
#include "fs/dev/acpi_controller.hh"
#include "fs/buffer_manager.hh"
#include "hal/pci/pci_cfg_header.hh"
#include "hal/sata/hba_port.hh"
#include "mm/physical_memory_manager.hh"
#include "klib/common.hh"

#include <mem/page.hh>
#include <mem/virtual_memory.hh>
#include <hsai_global.hh>

namespace dev
{
	namespace pci
	{
		PciDriver k_pci_driver;

		void PciDriver::init( const char *lock_name )
		{
			_lock.init( lock_name );

			_sata_init();

			_acpi_init();
		}

		void PciDriver::_sata_init()
		{
			// // distribute command list and FIS buffer space 

			// uint page_cnt, port;
			// uint64 addr_base;

			// // command list buffer space 
			// page_cnt = ata::sata::max_port_num / ( hsai::page_size / ata::sata::hba_cmd_lst_len );
			// port = 0;
			// for ( uint i = 0; i < page_cnt; i++ )
			// {
			// 	addr_base = ( uint64 ) mm::k_pmm.alloc_page();
			// 	if ( addr_base == 0x0 )
			// 	{
			// 		log_trace( "alloc port %d command list space: no mem", port );
			// 		log_panic( "no mem" );
			// 	}
			// 	mm::k_pmm.clear_page( ( void* ) addr_base );
			// 	addr_base = hsai::k_mem->to_dma( addr_base );
			// 	for (
			// 		uint64 addr = ( uint64 ) addr_base;
			// 		addr < ( uint64 ) addr_base + hsai::page_size;
			// 		addr += ata::sata::hba_cmd_lst_len )
			// 	{
			// 		sata::k_sata_driver.set_port_clb( port, ( void* ) addr );
			// 		++port;
			// 	}
			// }

			// // FIS buffer space 
			// page_cnt = ata::sata::max_port_num / ( hsai::page_size / ata::sata::hba_rec_fis_len );
			// port = 0;
			// for ( uint i = 0; i < page_cnt; i++ )
			// {
			// 	addr_base = ( uint64 ) mm::k_pmm.alloc_page();
			// 	if ( addr_base == 0x0 )
			// 	{
			// 		log_trace( "alloc port %d receive FIS space: no mem", port );
			// 		log_panic( "no mem" );
			// 	}
			// 	mm::k_pmm.clear_page( ( void* ) addr_base );
			// 	addr_base = hsai::k_mem->to_dma( addr_base );
			// 	for (
			// 		uint64 addr = ( uint64 ) addr_base;
			// 		addr < ( uint64 ) addr_base + hsai::page_size;
			// 		addr += ata::sata::hba_rec_fis_len )
			// 	{
			// 		sata::k_sata_driver.set_port_fb( port, ( void* ) addr );
			// 		++port;
			// 	}
			// }

			// // 读取 PCI 配置头
			// dev::pci::PciCfgHeader *pciHead = ( dev::pci::PciCfgHeader * ) loongarch::qemuls2k::PciCfgDevAddr::pci_cfg_sata;
			// if ( pciHead->vendor_id != 0x0014 )
			// 	log_panic( "PCI SATA配置头不合规 - vendor id is not 0x0014" );

			// // 从配置头获取 HBA Memory Registers base address 
			// uint64 sata_mem_base = pciHead->base_address[ 0 ];
			// sata_mem_base |= ( uint64 ) pciHead->base_address[ 1 ] << 32;
			// sata_mem_base |= loongarch::qemuls2k::dmwin::win_1;				// 使用非缓存窗口 
			// log_trace( "SATA 内部寄存器基地址: %p\n", sata_mem_base );
			// sata::k_sata_driver.set_hba_mem_reg( ( void* ) sata_mem_base );

			// log_info( "debug print clb and fb" );
			// // sata::k_sata_driver.debug_print_cmd_lst_base();
			// // sata::k_sata_driver.debug_print_rec_fis_base();

			// // 设置SATA端口数量，这里硬设置为3
			// sata::k_sata_driver.set_port_num( 3 );

			// sata::k_sata_driver.init( "sata" );

			// sata::k_sata_driver._logical_sector_size = 512/*Bytes*/;
			// log_warn( "使用固定扇区大小: %d Bytes", sata::k_sata_driver._logical_sector_size );

			// >>>>>>>> end

			// uchar buf[ 1024 ];
			// bool flag = false;
			// ahci::k_ahci_ctl.isu_cmd_identify( 0, ( void * ) buf, sizeof( buf ), [ & ] () -> void
			// {
			// 	log_info( "<<<<<<<< identify 回调 >>>>>>>>" );

			// 	uint16 sec_size_word = *( ( uint16 * ) buf + 106 );
			// 	log_trace( "106 word : %x", sec_size_word );
			// 	if ( ( sec_size_word >> 14 ) != 1U )
			// 		log_panic( "identify disk - 无效的返回数据" );
			// 	if ( sec_size_word & ( 1U << 12 ) )
			// 	{
			// 		uint32 logical_sec_size = *( ( uint16* ) buf + 117 );
			// 		logical_sec_size += *( ( uint16* ) buf + 118 ) << 16;
			// 		sata::k_sata_driver._logical_sector_size = logical_sec_size;
			// 	}
			// 	else
			// 	{
			// 		sata::k_sata_driver._logical_sector_size = 512/*bytes*/;
			// 	}
			// 	log_trace( "identify disk - 逻辑扇区大小 %d bytes", sata::k_sata_driver.get_sector_size() );
			// 	assert( sata::k_sata_driver._logical_sector_size == fs::default_sector_size,
			// 		"buffer使用了默认的扇区大小, 但是识别SATA设备信息与此不符\n"
			// 		">> 默认 %d Bytes 但是识别为 %d Bytes",
			// 		sata::k_sata_driver._logical_sector_size,
			// 		fs::default_sector_size
			// 	);

			// 	uint16 queue_depth = *( ( uint16 * ) buf + 75 );
			// 	uint16 sata_cap = *( ( uint16 * ) buf + 76 );
			// 	uint16 feature = *( ( uint16 * ) buf + 83 );
			// 	log_trace(
			// 		"word 76 : %x\n"
			// 		"word 83 : %x\n"
			// 		"command support : %s %s\n"
			// 		"queue depth : %d",
			// 		sata_cap,
			// 		feature,
			// 		( feature & ( 0x1U << 1 ) ) ? "DMA-QEUEUD" : "",
			// 		( sata_cap & ( 0x1U << 8 ) ) ? "FPDMA" : "",
			// 		queue_depth
			// 	);

			// 	flag = true;
			// } );
			// while ( !flag );
			// log_info( ">>>>>>>> flag change <<<<<<<<" );
		}

		void PciDriver::_acpi_init()
		{
			acpi::k_acpi_controller.init( "ACPI controller", hsai::k_mem->to_io( 0x1fe27000UL ) );
		}


	} // namespace pci

} // namespace dev

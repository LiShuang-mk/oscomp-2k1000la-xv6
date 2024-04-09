//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/sata/sata_ls2k.hh"
#include "hal/sata/hba_mem.hh"
#include "hal/sata/hba_port.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/pci/pci_cfg_header.hh"
#include "klib/common.hh"

namespace ata
{
	namespace sata
	{
		SataLs2k k_sata_driver;

		void SataLs2k::init( const char * lock_name, void *clb, void *fb )
		{
			SataDriver::init( lock_name, clb, fb );

			pci::PciCfgHeader *pciHead = ( pci::PciCfgHeader * ) loongarch::qemuls2k::PciCfgDevAddr::pci_cfg_sata;
			printf( "%x\n", pciHead->vendor_id );
			if ( pciHead->vendor_id != 0x0014 )
				log_panic( "pci head - vendor id is not 0x0014" );

			uint64 sata_mem_base = pciHead->base_address[ 0 ];
			sata_mem_base |= (uint64)pciHead->base_address[ 1 ] << 32;
			sata_mem_base |= loongarch::qemuls2k::dmwin::win_1;
			printf( "SATA Memory Registers Base: %p\n", sata_mem_base );

			ata::sata::HbaMemReg *hba_mem_reg = ( ata::sata::HbaMemReg * ) sata_mem_base;
			printf( "SATA CAP: %x\n", hba_mem_reg->cap );
			printf( "SATA Ports Implemented: %x\n", hba_mem_reg->pi );
			printf( "SATA AHCI Version: %x\n", hba_mem_reg->vs );

			ata::sata::HbaPortReg *hba_port_reg = ( ata::sata::HbaPortReg * ) &( hba_mem_reg->ports[ 0 ] );
			
		}
	} // namespace sata

} // namespace ata

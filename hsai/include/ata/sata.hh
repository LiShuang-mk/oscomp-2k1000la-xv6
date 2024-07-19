//
// Created by Li Shuang ( pseudonym ) on 2024-07-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "ata_types.hh"

namespace hsai
{
	/*****************************************
	 * SATA 协议相关定义
	 * 参考：Serial ATA Revision 2.6
	 *****************************************/

	/// @brief refer to https://wiki.osdev.org/AHCI#Detect_attached_SATA_devices
	enum SataSig : u32
	{
		sata_sig_ata = 0x00000101,
		sata_sig_atapi = 0xeb140101,
		sata_sig_semb = 0xc33c0101,
		sata_sig_pm = 0x96690101,
	};

	/// @brief FIS type value enum.
	/// refer to SerialATA revision 2.6 - 10.3.1 FIS Type Values 
	enum SataFisType : byte
	{
		sata_fis_reg_h2d = 0x27,
		sata_fis_reg_d2h = 0x34,
		sata_fis_dma_d2h = 0x39,
		sata_fis_dma_bid = 0x41,
		sata_fis_dat_bid = 0x46,
		sata_fis_bis_bid = 0x58,
		sata_fis_pio_d2h = 0x5F,
		sata_fis_sdb_d2h = 0xA1,
	};

	/// @brief refer to SerialATA revision 2.6 - 10.3.4 Register Host to Device 
	struct SataFisRegH2D
	{
		// Dword 0

		byte fis_type = sata_fis_reg_h2d;
		byte pm_port : 4;
		byte rsv0 : 3;					// [x] reserve 
		byte c : 1;
		byte command;
		byte features;

		// Dword 1 

		byte lba_low;
		byte lba_mid;
		byte lba_high;
		byte device;

		// Dword 2 

		byte lba_low_exp;
		byte lba_mid_exp;
		byte lba_high_exp;
		byte features_exp;

		// Dword 3 

		byte sector_cnt;
		byte sector_cnt_exp;
		byte rsv1;						// [x] reserve 
		byte control;

		// Dword 4 
		byte rsv2[ 4 ];					// [x] reserve 
	}__attribute__( ( __packed__ ) );
	static_assert( sizeof( SataFisRegH2D ) == 20 );

	/// @brief refer to SerialATA revision 2.6 - 10.3.10 PIO Setup Device to Host
	struct SataFisPioD2H
	{
		// Dword 0

		byte fis_type = sata_fis_pio_d2h;
		byte pm_port : 4;
		byte rsv0 : 1;					// [x] reserve 
		byte d : 1;
		byte i : 1;
		byte rsv1 : 1;					// [x] reserve 
		byte status;
		byte error;

		// Dword 1 

		byte lba_low;
		byte lba_mid;
		byte lba_high;
		byte device;

		// Dword 2 

		byte lba_low_exp;
		byte lba_mid_exp;
		byte lba_high_exp;
		byte rsv2;

		// Dword 3 

		byte sector_cnt;
		byte sector_cnt_exp;
		byte rsv3;						// [x] reserve 
		byte e_status;

		// Dword 4 
		word transfer_cnt;
		byte rsv4[ 2 ];					// [x] reserve 
	}__attribute__( ( __packed__ ) );
	static_assert( sizeof( SataFisPioD2H ) == 20 );

} // namespace hsai

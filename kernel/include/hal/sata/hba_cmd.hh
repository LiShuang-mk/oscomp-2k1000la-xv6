//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hal/ata/ata_types.hh"
#include "hal/sata/hba_param.hh"

namespace ata
{
	namespace sata
	{
		/// @brief refer to AHCI - 4.2.2 Command List Structure 
		struct HbaCmdHeader
		{
			// DW0 

			byte cfl : 5;		// Commmand FIS Length 
			byte a : 1;			// ATAPI 
			byte w : 1;			// Write 
			byte p : 1;			// Prefetchable 
			byte r : 1;			// Reset 
			byte b : 1;			// BIST 
			byte c : 1;			// Clear Busy upon R_OK 
			byte rsv : 1; 		// [x] reserve 
			byte pmp : 3;		// Port Multiplier Port 
			word prdtl;			// Physical Region Descriptor Table Length 

			// DW1 

			dword prdbc;		// PRD byte count 

			// DW2 

			dword ctba;			// Command Table Descriptor Base Address : aligned to 128-byte 

			// DW3 

			dword ctbau;		// CTBA Upper 32-bit

			// DW 4-7 
			
			dword rsv_dword[ 4 ];	// [x] reserve 
		};

		constexpr uint hba_cmd_list_size = 32;

		/// @brief refer to AHCI - 4.2.2 Command List Structure
		struct HbaCmdList
		{
			HbaCmdHeader headers[ hba_cmd_list_size ];
		};

	} // namespace sata
	
} // namespace ata

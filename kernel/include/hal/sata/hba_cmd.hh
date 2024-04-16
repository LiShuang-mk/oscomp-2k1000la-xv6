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
#include "mm/page.hh"

namespace ata
{
	namespace sata
	{
		struct HbaPrd;


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
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct HbaCmdHeader ) == 0x20U, "AHCI command header size must be 0x20 bytes." );

		constexpr uint hba_cmd_list_length = 32;

		/// @brief refer to AHCI - 4.2.2 Command List Structure
		struct HbaCmdList
		{
			HbaCmdHeader headers[ hba_cmd_list_length ];
		}__attribute__( ( __packed__ ) );

		/// @brief physical region descriptor
		/// refer to AHCI - 4.2.3 Command Table 
		struct HbaPrd
		{
			uint64 dba;				// data base address 
			uint32 resv1;
			uint8 interrupt : 1;
			uint16 resv2 : 9;
			uint32 dbc : 22;		// data byte count 
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct HbaPrd ) == 4 * sizeof( uint32 ), "PRD size must equal to 16 bytes." );


		/// @brief refer to AHCI - 4.2.3 Command Table 
		struct HbaCmdTbl
		{
			byte cmd_fis[ 0x40U ];
			byte acmd[ 0x10U ];
			byte resv[ 0x30U ];
			struct HbaPrd prdt[ ( mm::pg_size - 0x80U ) / sizeof( struct HbaPrd ) ];
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct HbaCmdTbl ) == mm::pg_size, "AHCI command table size must equal to page size." );


	} // namespace sata

} // namespace ata

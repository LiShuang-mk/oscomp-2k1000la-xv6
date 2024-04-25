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
			qword dba;				// data base address 
			dword resv1;
			dword dbc : 22;		// data byte count 
			dword resv2 : 9;
			dword interrupt : 1;
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct HbaPrd ) == 4 * sizeof( uint32 ), "PRD size must equal to 16 bytes." );

		constexpr uint prd_max_cnt = ( mm::pg_size - 0x80U ) / sizeof( struct HbaPrd );
		/// @brief refer to AHCI - 4.2.3 Command Table 
		/// @details 注意虽然文档中说明了PRD最多有65535个item
		///          但此处使item填满一个物理页剩下的空间
		///          计算得出一个物理页（4K）可以填下248个PRD
		struct HbaCmdTbl
		{
			byte cmd_fis[ 0x40U ];
			byte acmd[ 0x10U ];
			byte resv[ 0x30U ];
			struct HbaPrd prdt[ prd_max_cnt ];
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct HbaCmdTbl ) == mm::pg_size, "AHCI command table size must equal to page size." );
	} // namespace sata

} // namespace ata

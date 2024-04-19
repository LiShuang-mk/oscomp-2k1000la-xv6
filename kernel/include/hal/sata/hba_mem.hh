//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

/**
 * 这个文件用于抽象HBA(Host Bus Adapter)
 * 详细可参考 AHCI文档
*/

#pragma once 

#include "hal/ata/ata_types.hh"
#include "hal/sata/hba_param.hh"
#include "hal/sata/hba_port.hh"

namespace ata
{
	namespace sata
	{
		/// @brief refer to AHCI - 3. HBA Memory Register
		struct HbaMemReg
		{
			// generic host control 

			dword cap;
			dword ghc;
			dword is;
			dword pi;
			dword vs;
			dword ccc_ctl;
			dword ccc_ports;
			dword em_loc;
			dword em_ctl;
			dword cap2;
			dword bohc;

			// reserve from 2Ch to 5Fh
			byte _resv[ 0x60 - 0x2C ];

			// reserve from 60h to 9Fh for NVMHCI 
			byte _resv_nvmhci[ 0xA0 - 0x60 ];

			// vendor specific register 
			// from A0h to FFh
			byte vendor_reg[ 0x100 - 0x0A0 ];

			HbaPortReg ports[ max_port_num ];
		}__attribute__( ( __packed__ ) );
		// static_assert( ( uint64 )( ( ( HbaMemReg* ) 0x0UL )->ports ) == 0x100 );

#define _build_cap_bit_(name, mask, shift) \
	hba_cap_##name##_s = shift, \
	hba_cap_##name##_m = mask << hba_cap_##name##_s, 

		/// @brief HBA - Capabilities Register bit enum
		/// refer to AHCI 3.1.1 - revision 1.3.1 
		enum HbaRegCap : uint32
		{
			_build_cap_bit_( s64a, 0x1U, 31 )
			_build_cap_bit_( hncq, 0x1U, 30 )
			_build_cap_bit_( ssntf, 0x1U, 29 )
			_build_cap_bit_( smps, 0x1U, 28 )
			_build_cap_bit_( sss, 0x1U, 27 )
			_build_cap_bit_( salp, 0x1U, 26 )
			_build_cap_bit_( sal, 0x1U, 25 )
			_build_cap_bit_( sclo, 0x1U, 24 )
			_build_cap_bit_( iss, 0xFU, 20 )
			_build_cap_bit_( snzo, 0x1U, 19 )
			_build_cap_bit_( sam, 0x1U, 18 )
			_build_cap_bit_( spm, 0x1U, 17 )
			_build_cap_bit_( fbss, 0x1U, 16 )
			_build_cap_bit_( pmd, 0x1U, 15 )
			_build_cap_bit_( ssc, 0x1U, 14 )
			_build_cap_bit_( psc, 0x1U, 13 )
			_build_cap_bit_( ncs, 0x1FU, 8 )
			_build_cap_bit_( cccs, 0x1U, 7 )
			_build_cap_bit_( ems, 0x1U, 6 )
			_build_cap_bit_( sxs, 0x1U, 5 )
			_build_cap_bit_( np, 0x1FU, 0 )
		};
#undef _build_cap_bit_ 

#define _build_ghc_bit_(name, mask, shift) \
	hba_ghc_##name##_s = shift, \
	hba_ghc_##name##_m = mask << hba_ghc_##name##_s, 

		/// @brief HBA- global HBA control Register bit enum 
		enum HbaRegGhc : uint32
		{
			_build_ghc_bit_( ae, 0x1U, 31 )
			_build_ghc_bit_( mrsm, 0x1U, 2 )
			_build_ghc_bit_( ie, 0x1U, 1 )
			_build_ghc_bit_( hr, 0x1U, 0 )
		};

#undef _build_ghc_bit_

		

	} // namespace sata

} // namespace ata

//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hba_param.hh"

namespace ata
{
	namespace sata
	{
		/// @brief refer to AHCI - 3.3 Port Registers 
		struct  HbaPortReg
		{
			dword clb;
			dword clbu;
			dword fb;
			dword fbu;
			dword is;
			dword ie;
			dword cmd;
			dword tfd;
			dword sig;
			dword ssts;
			dword sctl;
			dword serr;
			dword sact;
			dword ci;
			dword sntf;
			dword fbs;
			dword devslp;

			// vendor specific from 70h to 7fh 
			byte vendor_reg[ 0x80 - 0x70 ];
		}__attribute__( ( __packed__ ) );

		constexpr uint hba_cmd_lst_len = 0x400U;
		constexpr uint hba_rec_fis_len = 0x100U;

#define _build_cmd_bit_(name, mask, shift) \
	hba_port_cmd_##name##_s = shift, \
	hba_port_cmd_##name##_m = mask << hba_port_cmd_##name##_s, 

		enum HbaRegPortCmd : uint32
		{
			_build_cmd_bit_( icc, 0xFU, 28 )
			_build_cmd_bit_( asp, 0x1U, 27 )
			_build_cmd_bit_( alpe, 0x1U, 26 )
			_build_cmd_bit_( dlae, 0x1U, 25 )
			_build_cmd_bit_( atapi, 0x1U, 24 )
			_build_cmd_bit_( apste, 0x1U, 23 )
			_build_cmd_bit_( fbscp, 0x1U, 22 )
			_build_cmd_bit_( esp, 0x1U, 21 )
			_build_cmd_bit_( cpd, 0x1U, 20 )
			_build_cmd_bit_( mpsp, 0x1U, 19 )
			_build_cmd_bit_( hpcp, 0x1U, 18 )
			_build_cmd_bit_( pma, 0x1U, 17 )
			_build_cmd_bit_( cps, 0x1U, 16 )
			_build_cmd_bit_( cr, 0x1U, 15 )
			_build_cmd_bit_( fr, 0x1U, 14 )
			_build_cmd_bit_( mpss, 0x1U, 13 )
			_build_cmd_bit_( ccs, 0x1FU, 8 )
			_build_cmd_bit_( fre, 0x1U, 4 )
			_build_cmd_bit_( clo, 0x1U, 3 )
			_build_cmd_bit_( pod, 0x1U, 2 )
			_build_cmd_bit_( sud, 0x1U, 1 )
			_build_cmd_bit_( st, 0x1U, 0 )
		};
#undef _build_cmd_bit_ 

#define _build_ssts_bit_(name, mask, shift) \
	hba_port_ssts_##name##_s = shift, \
	hba_port_ssts_##name##_m = mask << hba_port_ssts_##name##_s,

		enum HbaRegPortSsts : uint32
		{
			_build_ssts_bit_( ipm, 0xFU, 8 )
			_build_ssts_bit_( spd, 0xFU, 4 )
			_build_ssts_bit_( det, 0xFU, 0 )
		};
#undef _build_ssts_bit_

#define _build_serr_bit_( name, mask, shift ) \
	hba_port_serr_##name##_s = shift, \
	hba_port_ssts_##name##_m = mask << hba_port_serr_##name##_s, 

		enum HbaRegPortSerr : uint32
		{
			_build_serr_bit_( diag, 0xFFFFU, 16 )
			_build_serr_bit_( diag_x, 0x1U, 26 )
			_build_serr_bit_( diag_f, 0x1U, 25 )
			_build_serr_bit_( diag_t, 0x1U, 24 )
			_build_serr_bit_( diag_s, 0x1U, 23 )
			_build_serr_bit_( diag_h, 0x1U, 22 )
			_build_serr_bit_( diag_c, 0x1U, 21 )
			_build_serr_bit_( diag_d, 0x1U, 20 )
			_build_serr_bit_( diag_b, 0x1U, 19 )
			_build_serr_bit_( diag_w, 0x1U, 18 )
			_build_serr_bit_( diag_i, 0x1U, 17 )
			_build_serr_bit_( diag_n, 0x1U, 16 )

			_build_serr_bit_( err, 0xFFFFU, 0 )
			_build_serr_bit_( err_e, 0x1U, 11 )
			_build_serr_bit_( err_p, 0x1U, 10 )
			_build_serr_bit_( err_c, 0x1U, 9 )
			_build_serr_bit_( err_t, 0x1U, 8 )
			_build_serr_bit_( err_m, 0x1U, 1 )
			_build_serr_bit_( err_i, 0x1U, 0 )
		};
#undef _build_serr_bit_

#define _build_ie_bit_( name, mask, shift ) \
	hba_port_ie_##name##_s = shift, \
	hba_port_ie_##name##_m = mask << hba_port_ie_##name##_s, 

		enum HbaRegPortIe : uint32
		{
			_build_ie_bit_( cpde, 0x1U, 31 )
			_build_ie_bit_( tfee, 0x1U, 30 )
			_build_ie_bit_( hbfe, 0x1U, 29 )
			_build_ie_bit_( hbde, 0x1U, 28 )
			_build_ie_bit_( ife, 0x1U, 27 )
			_build_ie_bit_( infe, 0x1U, 26 )
			_build_ie_bit_( ofe, 0x1U, 24 )
			_build_ie_bit_( ipme, 0x1U, 23 )
			_build_ie_bit_( prce, 0x1U, 22 )
			_build_ie_bit_( dmpe, 0x1U, 7 )
			_build_ie_bit_( pce, 0x1U, 6 )
			_build_ie_bit_( dpe, 0x1U, 5 )
			_build_ie_bit_( ufe, 0x1U, 4 )
			_build_ie_bit_( sdbe, 0x1U, 3 )
			_build_ie_bit_( dse, 0x1U, 2 )
			_build_ie_bit_( pse, 0x1U, 1 )
			_build_ie_bit_( dhre, 0x1U, 0 )
		};
#undef _build_ie_bit_ 

	} // namespace sata

} // namespace ata

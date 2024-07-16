//
// Created by Li Shuang ( pseudonym ) on 2024-07-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "ata_types.hh"
#include "mem/page.hh"

namespace hsai
{
	/*****************************************
	 * AHCI 协议相关定义
	 * 参考：Serial ATA Advanced Host Controller
	 *       Interface Revision 1.3.1
	 *****************************************/

	constexpr uint max_port_num = 32;
	constexpr uint max_cmd_slot = 32;

	///////////////////////////////////////////////////////////////////////////////////
	// HBA 内部寄存器相关定义

	/// @brief refer to AHCI 1.3.1 - 3.1 Generic Host Control
	struct AhciGenericReg
	{
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
	};
	static_assert( sizeof( AhciGenericReg ) == 0x2C, "AHCI Generic Host Control registers size shall be 0x2C bytes" );

#define _build_cap_bit_(name, mask, shift) \
	ahci_cap_##name##_s = shift, \
	ahci_cap_##name##_m = mask << ahci_cap_##name##_s, 
	/// @brief refer to AHCI 1.3.1 - 3.1.1 CAP
	enum AhciRegCap : dword
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
	ahci_ghc_##name##_s = shift, \
	ahci_ghc_##name##_m = mask << ahci_ghc_##name##_s, 
	/// @brief refer to AHCI 1.3.1 - 3.1.2 GHC
	enum AhciRegGhc : dword
	{
		_build_ghc_bit_( ae, 0x1U, 31 )
		_build_ghc_bit_( mrsm, 0x1U, 2 )
		_build_ghc_bit_( ie, 0x1U, 1 )
		_build_ghc_bit_( hr, 0x1U, 0 )
	};

#undef _build_ghc_bit_

	/// @brief refer to AHCI 1.3.1 - 3.3 Port Registers 
	struct AhciPortReg
	{
		dword clb;
		dword clbu;
		dword fb;
		dword fbu;
		dword is;
		dword ie;
		dword cmd;
		dword tfd;
		dword resv0;
		dword sig;
		dword ssts;
		dword sctl;
		dword serr;
		dword sact;
		dword ci;
		dword sntf;
		dword fbs;
		dword devslp;

		// reserved 
		byte resv1[ 0x70 - 0x48 ];

		// vendor specific from 70h to 7fh 
		byte vendor_reg[ 0x80 - 0x70 ];

	}__attribute__( ( __packed__ ) );
	static_assert( sizeof( AhciPortReg ) == 0x80 );

	constexpr uint ahci_cmd_lst_len = 0x400U;
	constexpr uint ahci_rec_fis_len = 0x100U;

#define _build_is_bit_( name, mask, shift ) \
	ahci_port_is_##name##_s = shift, \
	ahci_port_is_##name##_m = mask << ahci_port_is_##name##_s,
	/// @brief refer to AHCI 1.3.1 - 3.3.5 PxIS
	enum AhciRegPortIs : dword
	{
		_build_is_bit_( cpds, 0x1U, 31 )
		_build_is_bit_( tfes, 0x1U, 30 )
		_build_is_bit_( hbfs, 0x1U, 29 )
		_build_is_bit_( hbds, 0x1U, 28 )
		_build_is_bit_( ifs, 0x1U, 27 )
		_build_is_bit_( infs, 0x1U, 26 )
		_build_is_bit_( ofs, 0x1U, 24 )
		_build_is_bit_( ipms, 0x1U, 23 )
		_build_is_bit_( prcs, 0x1U, 22 )
		_build_is_bit_( dmps, 0x1U, 7 )
		_build_is_bit_( pcs, 0x1U, 6 )
		_build_is_bit_( dps, 0x1U, 5 )
		_build_is_bit_( ufs, 0x1U, 4 )
		_build_is_bit_( sdbs, 0x1U, 3 )
		_build_is_bit_( dss, 0x1U, 2 )
		_build_is_bit_( pss, 0x1U, 1 )
		_build_is_bit_( dhrs, 0x1U, 0 )
	};
#undef _build_ie_bit_

#define _build_ie_bit_( name, mask, shift ) \
	ahci_port_ie_##name##_s = shift, \
	ahci_port_ie_##name##_m = mask << ahci_port_ie_##name##_s, 
	/// @brief refer to AHCI 1.3.1 - 3.3.6 PxIE
	enum AhciRegPortIe : dword
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

#define _build_cmd_bit_(name, mask, shift) \
	ahci_port_cmd_##name##_s = shift, \
	ahci_port_cmd_##name##_m = mask << ahci_port_cmd_##name##_s,
	/// @brief refer to AHCI 1.3.1 - 3.3.7 PxCMD
	enum AhciRegPortCmd : dword
	{
		_build_cmd_bit_( icc, 0xFU, 28 )
		_build_cmd_bit_( icc_devsleep, 0x8U, 28 )
		_build_cmd_bit_( icc_slumber, 0x6U, 28 )
		_build_cmd_bit_( icc_partial, 0x2U, 28 )
		_build_cmd_bit_( icc_active, 0x1U, 28 )
		_build_cmd_bit_( icc_idle, 0x0U, 28 )
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

#define _build_tfd_bit_( name, mask, shift ) \
	ahci_port_tfd_##name##_s = shift, \
	ahci_port_tfd_##name##_m = mask << ahci_port_tfd_##name##_s, 
	/// @brief refer to AHCI 1.3.1 - 3.3.8 PxTFD
	enum AhciRegPortTfd : dword
	{
		_build_tfd_bit_( err, 0xFFU, 8 )
		_build_tfd_bit_( sts, 0xFFU, 0 )
		_build_tfd_bit_( sts_bsy, 0x1U, 7 )
		_build_tfd_bit_( sts_drq, 0x1U, 3 )
		_build_tfd_bit_( sts_err, 0x1U, 0 )
	};
#undef _build_tfd_bit_

#define _build_ssts_bit_(name, mask, shift) \
	ahci_port_ssts_##name##_s = shift, \
	ahci_port_ssts_##name##_m = mask << ahci_port_ssts_##name##_s,
	/// @brief refer to AHCI 1.3.1 - 3.3.10 PxSSTS
	enum AhciRegPortSsts : dword
	{
		_build_ssts_bit_( ipm, 0xFU, 8 )
		_build_ssts_bit_( spd, 0xFU, 4 )
		_build_ssts_bit_( det, 0xFU, 0 )
	};
#undef _build_ssts_bit_

#define _build_sctl_bit_(name, mask, shift) \
	ahci_port_sctl_##name##_s = shift, \
	ahci_port_sctl_##name##_m = mask << ahci_port_sctl_##name##_s,
	/// @brief refer to AHCI 1.3.1 - 3.3.11 PxSCTL
	enum AhciRegPortSctl : dword
	{
		_build_sctl_bit_( ipm, 0xFU, 8 )
		_build_sctl_bit_( spd, 0xFU, 4 )
		_build_sctl_bit_( det, 0xFU, 0 )
	};
#undef _build_sctl_bit_

#define _build_serr_bit_( name, mask, shift ) \
	ahci_port_serr_##name##_s = shift, \
	ahci_port_serr_##name##_m = mask << ahci_port_serr_##name##_s, 
	/// @brief refer to AHCI 1.3.1 - 3.3.12 PxSERR
	enum AhciRegPortSerr : dword
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

	/// @brief refer to AHCI 1.3.1 - 3. HBA Memory Register
	struct AhciMemReg
	{
		// generic host control 
		AhciGenericReg generic;

		// reserve from 2Ch to 5Fh
		byte _resv[ 0x60 - 0x2C ];

		// reserve from 60h to 9Fh for NVMHCI 
		byte _resv_nvmhci[ 0xA0 - 0x60 ];

		// vendor specific register 
		// from A0h to FFh
		byte vendor_reg[ 0x100 - 0x0A0 ];

		AhciPortReg ports[ max_port_num ];
	}__attribute__( ( __packed__ ) );
	// static_assert( ( uint64 )( ( ( HbaMemReg* ) 0x0UL )->ports ) == 0x100 );



	///////////////////////////////////////////////////////////////////////////////////
	// 系统内存结构相关定义

	/// @brief refer to AHCI 1.3.1 - 4.2.2 Command List Structure 
	struct AhciCmdHeader
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
	static_assert( sizeof( AhciCmdHeader ) == 0x20U, "AHCI command header size must be 0x20 bytes." );

	constexpr uint ahci_cmd_list_length = 32;

	/// @brief refer to AHCI - 4.2.2 Command List Structure
	struct HbaCmdList
	{
		AhciCmdHeader headers[ ahci_cmd_list_length ];
	}__attribute__( ( __packed__ ) );

	/// @brief physical region descriptor
	/// refer to AHCI - 4.2.3 Command Table 
	struct AhciPrd
	{
		qword dba;				// data base address 
		dword resv1;
		dword dbc : 22;		// data byte count 
		dword resv2 : 9;
		dword interrupt : 1;
	}__attribute__( ( __packed__ ) );
	static_assert( sizeof( AhciPrd ) == 16, "PRD size must equal to 16 bytes." );

	constexpr uint ahci_prd_max_cnt = ( hsai::page_size - 0x80U ) / sizeof( struct AhciPrd );
	/// @brief refer to AHCI - 4.2.3 Command Table 
	/// @details 注意虽然文档中说明了PRD最多有65535个item
	///          但此处使item填满一个物理页剩下的空间
	///          计算得出一个物理页（4K）可以填下248个PRD
	struct AhciCmdTbl
	{
		byte cmd_fis[ 0x40U ];
		byte acmd[ 0x10U ];
		byte resv[ 0x30U ];
		struct AhciPrd prdt[ ahci_prd_max_cnt ];
	}__attribute__( ( __packed__ ) );
	static_assert( sizeof( AhciCmdTbl ) == hsai::page_size, "AHCI command table size must equal to page size." );

} // namespace hsai

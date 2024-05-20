//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

extern "C" {
#include "hal/laregs.h"
}

namespace loongarch
{
	namespace csr
	{
		enum CsrAddr : uint64
		{
			crmd = 0x0,
			prmd = 0x1,
			euen = 0x2,
			misc = 0x3,
			ecfg = 0x4,
			estat = 0x5,
			era = 0x6,
			badv = 0x7,
			badi = 0x8,
			eentry = 0xc,
			tlbidx = 0x10,
			tlbehi = 0x11,
			tlbelo0 = 0x12,
			tlbelo1 = 0x13,
			asid = 0x18,
			pgdl = 0x19,
			pgdh = 0x1a,
			pgd = 0x1b,
			pwcl = 0x1c,
			pwch = 0x1d,
			stlbps = 0x1e,
			rvacfg = 0x1f,
			cpuid = 0x20,
			prcfg1 = 0x21,
			prcfg2 = 0x22,
			prcfg3 = 0x23,
			save0 = 0x30,
			tid = 0x40,
			tcfg = 0x41,
			tval = 0x42,
			cntc = 0x43,
			ticlr = 0x44,
			llbctl = 0x60,
			impctl1 = 0x80,
			impctl2 = 0x81,
			tlbrentry = 0x88,
			tlbrbadv = 0x89,
			tlbrera = 0x8a,
			tlbrsave = 0x8b,
			tlbrelo0 = 0x8c,
			tlbrelo1 = 0x8d,
			tlbrehi = 0x8e,
			tlbrprmd = 0x8f,
			merrctl = 0x90,
			merrinfo1 = 0x91,
			merrinfo2 = 0x92,
			merrentry = 0x93,
			merrera = 0x94,
			merrsave = 0x95,
			ctag = 0x98,
			msgis0 = 0xa0,
			msgis1 = 0xa1,
			msgis2 = 0xa2,
			msgis3 = 0xa3,
			msgir = 0xa4,
			msgie = 0xa5,
			dmwin0 = 0x180,
			dmwin1 = 0x181,
			dmwin2 = 0x182,
			dmwin3 = 0x183,
		};

		/// @brief	s: shift;
		///			m: mask;
		enum Crmd : uint32
		{
			plv_s = 0x0,
			plv_m = 0x3 << plv_s,
			ie_s = 0x2,
			ie_m = 0x1 << ie_s,
			da_s = 0x3,
			da_m = 0x1 << da_s,
			pg_s = 0x4,
			pg_m = 0x1 << pg_s,
			datf_s = 0x5,
			datf_m = 0x3 << datf_s,
			datm_s = 0x7,
			datm_m = 0x3 << datm_s,
			we_s = 0x9,
			we_m = 0x1 << we_s,
		};

		/// @brief s: shift;
		///		   m: mask;	
		enum Tcfg : uint64
		{
			tcfg_en_s = 0,
			tcfg_en_m = 0x1UL << tcfg_en_s,
			tcfg_periodic_s = 1,
			tcfg_periodic_m = 0x1UL << tcfg_periodic_s,
			tcfg_initval_s = 2,
			tcfg_initval_m = 0x3fffffffffffffffUL << tcfg_initval_s
		};

		/// @brief 13个中断源的枚举
		/// s: shift;
		/// m: mask;
		enum itr : uint64
		{
			itr_ipi_s = 12,
			itr_ti_s = 11,
			itr_pmi_s = 10,
			itr_hwi7_s = 9,
			itr_hwi6_s = 8,
			itr_hwi5_s = 7,
			itr_hwi4_s = 6,
			itr_hwi3_s = 5,
			itr_hwi2_s = 4,
			itr_hwi1_s = 3,
			itr_hwi0_s = 2,
			itr_hwi_s = itr_hwi0_s,
			itr_swi1_s = 1,
			itr_swi0_s = 0,
			itr_swi_s = itr_swi0_s,

			itr_ipi_m = 0x1 << itr_ipi_s,
			itr_ti_m = 0x1 << itr_ti_s,
			itr_pmi_m = 0x1 << itr_pmi_s,
			itr_hwi7_m = 0x1 << itr_hwi7_s,
			itr_hwi6_m = 0x1 << itr_hwi6_s,
			itr_hwi5_m = 0x1 << itr_hwi5_s,
			itr_hwi4_m = 0x1 << itr_hwi4_s,
			itr_hwi3_m = 0x1 << itr_hwi3_s,
			itr_hwi2_m = 0x1 << itr_hwi2_s,
			itr_hwi1_m = 0x1 << itr_hwi1_s,
			itr_hwi0_m = 0x1 << itr_hwi0_s,
			itr_hwi_m = 0xff << itr_hwi_s,
			itr_swi1_m = 0x1 << itr_swi1_s,
			itr_swi0_m = 0x1 << itr_swi0_s,
			itr_swi_m = 0x3 << itr_swi_s,
		};

		/// @brief 例外前模式信息
		enum prmd : uint64
		{
			pplv_s = 0,
			pie_s = 2,
			pwe_s = 3,

			pplv_m = 0x3 << pplv_s, 
			pie_m = 0x1 << pie_s,   
			pwe_m = 0x1 << pwe_s,   
		};

		/// @brief s: shift;
		///		   m: mask;
		enum ecfg : uint64
		{
			ecfg_lie_s = 0,
			ecfg_vs_s = 16,

			ecfg_lie_m = 0x1FFF << ecfg_lie_s,
			ecfg_vs_m = 0x7 << ecfg_vs_s,
		};

#define _build_estat_bit_( name, mask, shift ) \
	estat_##name##_s = shift, \
	estat_##name##_m = mask << estat_##name##_s, 
		enum Estat : uint32
		{
			_build_estat_bit_( is, 0x1FFFU, 0 )
			_build_estat_bit_( is_swi0, 0x1U, 0 )
			_build_estat_bit_( is_swi1, 0x1U, 1 )
			_build_estat_bit_( is_hwi0, 0x1U, 2 )
			_build_estat_bit_( is_hwi1, 0x1U, 3 )
			_build_estat_bit_( is_hwi2, 0x1U, 4 )
			_build_estat_bit_( is_hwi3, 0x1U, 5 )
			_build_estat_bit_( is_hwi4, 0x1U, 6 )
			_build_estat_bit_( is_hwi5, 0x1U, 7 )
			_build_estat_bit_( is_hwi6, 0x1U, 8 )
			_build_estat_bit_( is_hwi7, 0x1U, 9 )
			_build_estat_bit_( is_pmi, 0x1U, 10 )
			_build_estat_bit_( is_ti, 0x1U, 11 )
			_build_estat_bit_( is_ipi, 0x1U, 12 )
			_build_estat_bit_( msgint, 0x1U, 14 )
			_build_estat_bit_( ecode, 0x3FU, 16 )
			_build_estat_bit_( esubcode, 0x1FFU, 22 )
		};
#undef _build_estat_bit_ 

#define _build_ecode_enum_(name,code) \
	ecode_##name = code, 
		enum Ecode : uint
		{
			_build_ecode_enum_( int, 0x0 )
			_build_ecode_enum_( pil, 0x1 )
			_build_ecode_enum_( pis, 0x2 )
			_build_ecode_enum_( pif, 0x3 )
			_build_ecode_enum_( pme, 0x4 )
			_build_ecode_enum_( pnr, 0x5 )
			_build_ecode_enum_( pnx, 0x6 )
			_build_ecode_enum_( ppi, 0x7 )
			_build_ecode_enum_( ade, 0x8 )
			_build_ecode_enum_( ale, 0x9 )
			_build_ecode_enum_( bce, 0xA )
			_build_ecode_enum_( sys, 0xB )
			_build_ecode_enum_( brk, 0xC )
			_build_ecode_enum_( ine, 0xD )
			_build_ecode_enum_( ipe, 0xE )
			_build_ecode_enum_( fpd, 0xF )
			_build_ecode_enum_( sxd, 0x10 )
			_build_ecode_enum_( asxd, 0x11 )
			_build_ecode_enum_( fpe, 0x12 )
			_build_ecode_enum_( wpe, 0x13 )
			_build_ecode_enum_( btd, 0x14 )
			_build_ecode_enum_( bte, 0x15 )
			_build_ecode_enum_( gspr, 0x16 )
			_build_ecode_enum_( hvc, 0x17 )
			_build_ecode_enum_( gcc, 0x18 )
		};
#undef _build_ecode_enum_ 

		static inline uint64 _read_csr_( CsrAddr _csr )
		{
			return _la_r_csr_[ ( uint64 ) _csr ]();
		}

		static inline void _write_csr_( CsrAddr _csr, uint64 _data )
		{
			return _la_w_csr_[ ( uint64 ) _csr ]( _data );
		}

	} // namespace csr
} // namespace loongarch
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
			tlbehi = 0x11,
			asid = 0x18,
			pgdl = 0x19,
			pgdh = 0x1a,
			pgd = 0x1b,
			pwcl = 0x1c,
			pwch = 0x1d, 
			stlbps = 0x1e,
			cpuid = 0x20,
			save0 = 0x30,
			tlbrentry = 0x88,
			tlbrbadv = 0x89,
			tlbrsave = 0x8b,
			tlbrehi = 0x8e,
			dmwin0 = 0x180,
			dmwin1 = 0x181,
			dmwin2 = 0x182,
			dmwin3 = 0x183,
		};

		/// @brief	s: shift;
		///			m: mask;
		enum crmd : uint32
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
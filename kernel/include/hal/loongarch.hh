//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace loongarch
{
	struct Context
	{
		uint64 ra;
		uint64 sp;

		// callee-saved
		uint64 s0;
		uint64 s1;
		uint64 s2;
		uint64 s3;
		uint64 s4;
		uint64 s5;
		uint64 s6;
		uint64 s7;
		uint64 s8;
		uint64 fp;
	};

	enum PteEnum : uint64
	{
		valid_s = 0,
		dirty_s = 1,
		plv_s = 2,
		mat_s = 4,
		b_global_s = 6,		// base pte global bit shift 
		h_huge_s = 6, 		// huge pte huge flag bit shift 
		presence_s = 7,
		writable_s = 8,
		b_pa_s = 12,		// base pte physical address shift 
		h_global_s = 12, 	// huge pte global bit shift 
		h_pa_s = 15, 		// huge pte physical address shift 
		nr_s = 61,
		nx_s = 62,
		rplv_s = 63,

		valid_m = 0x1UL << valid_s,
		dirty_m = 0x1UL << dirty_s,
		plv_m = 0x3UL << plv_s,
		mat_m = 0x3UL << mat_s,
		b_global_m = 0x1UL << b_global_s,				// base pte global bit mask 
		h_huge_m = 0x1UL << h_huge_s, 				// huge pte huge flag bit mask 
		presence_m = 0x1UL << presence_s,
		writable_m = 0x1UL << writable_s,
		b_pa_m = 0x1FFFFFFFFFFFFUL << b_pa_s, 	// base pte pa mask [ 12...60 ]
		h_global_m = 0x1UL << h_global_s, 			// huge pte global bit mask 
		h_pa_m = 0x3FFFFFFFFFFFUL << h_pa_s, 	// huge pte pa mask [ 15..60 ]
		nr_m = 0x1UL << nr_s,
		nx_m = 0x1UL << nx_s,
		rplv_m = 0x1UL << rplv_s,
	};

	enum MatEnum : uint
	{
		mat_suc = 0x0,			// 强序非缓存
		mat_cc = 0x1,			// 一致可缓存
		mat_wuc = 0x2,			// 弱序非缓存
		mat_undefined = 0x3 
	};

} // namespace loongarch

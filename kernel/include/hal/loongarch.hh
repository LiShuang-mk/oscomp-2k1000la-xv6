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

	namespace qemuls2k
	{
		enum dmwin : uint64
		{
			win_0 = 0x9UL << 60,
		};

		/// @brief interface address 
		enum InterAddr : uint64
		{
			uart0 = 0x1fe20000UL | dmwin::win_0,
		};

		enum memory : uint64
		{
			mem_start = 0x90000000UL | dmwin::win_0,
			mem_size = CommonSize::_1M << 7, 			// 128M 
			mem_end = mem_start + mem_size
		};
	} // namespace qemuls2k


} // namespace loongarch

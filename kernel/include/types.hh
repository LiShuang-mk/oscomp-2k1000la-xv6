//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long;

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long;

using u8 = uint8;
using u16 = uint16;
using u32 = uint32;
using u64 = uint64;

using i8 = int8;
using i16 = int16;
using i32 = int32;
using i64 = int64;

using flag_t = uint64;
using pte_t = uint64;
using mode_t = uint32;
using dev_t = uint64;
#ifndef _SIZE_T
#define _SIZE_T
using size_t = uint64;
#endif
using wchar = ushort;

enum CommonSize : uint64
{
	_1K = 0x1UL << 10,
	_1M = 0x1UL << 20,
	_1G = 0x1UL << 30,
};

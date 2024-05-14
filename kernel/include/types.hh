//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

using uint = unsigned int;
using ushort = unsigned short;
using uchar = unsigned char;
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long;
using int64 = long;
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

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

enum CommonSize : uint64
{
	_1K = 0x1UL << 10,
	_1M = 0x1UL << 20,
	_1G = 0x1UL << 30,
	pg_size = _1K << 2, 		// 4KiB page 
};
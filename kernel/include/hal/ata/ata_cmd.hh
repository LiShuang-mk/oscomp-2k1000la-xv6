//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "ata_types.hh"

namespace ata
{
	enum AtaCmd : byte 
	{
		cmd_indentify_device = 0xEC,
		cmd_read_dma = 0x25, 
	};
} // namespace ata

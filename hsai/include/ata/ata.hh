//
// Created by Li Shuang ( pseudonym ) on 2024-07-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "ata_types.hh"

namespace hsai
{
	/*****************************************
	 * ATA 协议相关定义
	 * 参考：ATA8-ACS
	 *****************************************/

	/// @brief ATA 命令代码，参考 ATA8-ACS - 7 Command Description
	enum AtaCmd : byte
	{
		ata_cmd_identify_device = 0xEC,
		ata_cmd_read_dma = 0x25,
		ata_cmd_write_dma = 0x35,
	};

} // namespace hsai

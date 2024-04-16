//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace ata
{
	namespace sata
	{

#if PLATFORM == ls2k 
		constexpr uint hba_port_num = 32;
#else 
		constexpr uint hba_port_num = 1;
#endif
	} // namespace sata

} // namespace ata

//
// Created by Li Shuang ( pseudonym ) on 2024-05-11 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace fs
{
	namespace jbd2
	{
		struct JournalHeader
		{
			uint32 magic;
			uint32 blocktype;
			uint32 sequence;
		};

	} // namespace jbd2

} // namespace fs

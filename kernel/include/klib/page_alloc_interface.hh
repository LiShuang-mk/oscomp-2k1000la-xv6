//
// Created by Li Shuang ( pseudonym ) on 2024-05-01 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace klib
{
	class PageAllocator
	{
	public:
		virtual void * alloc_pages( uint cnt ) = 0;
		virtual void free_pages( void * ptr ) = 0;
	};
} // namespace klib

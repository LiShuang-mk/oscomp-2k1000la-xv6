//
// Created by Li Shuang ( pseudonym ) on 2024-07-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

namespace mm
{
	class VirtualPageAllocator
	{
	public:
		virtual void * alloc_pages( uint cnt ) = 0;
		virtual int free_pages( void * pages_ptr ) = 0;
	};

} // namespace mm

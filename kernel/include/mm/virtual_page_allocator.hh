//
// Created by Li Shuang ( pseudonym ) on 2024-07-17
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include <tuple>

namespace mm
{
	using MemUsage = u64;
	using MemRest  = u64;
	using MemDesc  = std::tuple<MemUsage, MemRest>;

	class VirtualPageAllocator
	{
	public:

		virtual void *	alloc_pages( ulong cnt )	   = 0;
		virtual int		free_pages( void * pages_ptr ) = 0;
		virtual MemDesc mem_desc()					   = 0;
	};

} // namespace mm

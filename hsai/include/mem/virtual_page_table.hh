//
// Created by Li Shuang ( pseudonym ) on 2024-07-12
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "mem/page.hh"

namespace hsai
{
	class VirtualPageTable __hsai_kernel
	{
	protected:
		ulong _pt_base = 0;

	public:
		virtual Pte walk( ulong va, bool alloc ) = 0;
		virtual ulong walk_addr( ulong va ) = 0;


	};

} // namespace hsai

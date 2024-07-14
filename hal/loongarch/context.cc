//
// Created by Li Shuang ( pseudonym ) on 2024-07-13 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "context.hh"

#include <process_interface.hh>

namespace loongarch
{
	Context k_proc_context_pool[ hsai::proc_pool_size ];

	Context * proc_context_pool() { return k_proc_context_pool; }
} // namespace loongarch

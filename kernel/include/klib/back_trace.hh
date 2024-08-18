//
// Created by Li Shuang ( pseudonym ) on 2024-08-18
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

namespace klib
{
	int back_trace_fp( void **ptr_buf, int ptr_max_cnt );
	int back_trace_ra( void **ptr_buf, int ptr_max_cnt );

} // namespace klib


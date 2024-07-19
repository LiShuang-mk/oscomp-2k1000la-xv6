//
// Created by Li Shuang ( pseudonym ) on 2024-07-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

namespace hsai
{
	extern __hsai_kernel void * alloc_pages( uint cnt );
	extern __hsai_kernel int free_pages( void * ptr );
	
} // namespace hsai

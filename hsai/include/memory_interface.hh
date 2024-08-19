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
	class VirtualPageTable;

	extern __hsai_kernel void *alloc_pages( uint cnt );
	extern __hsai_kernel int   free_pages( void *ptr );

	extern __hsai_kernel int copy_to_user( VirtualPageTable *pt, uint64 va, const void *p,
										   uint64 len );
	extern __hsai_kernel int copy_from_user( VirtualPageTable *pt, void *dst, uint64 src_va,
											 uint64 len );

} // namespace hsai

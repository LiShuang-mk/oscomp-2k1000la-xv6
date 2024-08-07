//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

namespace loongarch
{
	struct Context
	{
		uint64 ra;
		uint64 sp;

		// callee-saved
		uint64 s0;
		uint64 s1;
		uint64 s2;
		uint64 s3;
		uint64 s4;
		uint64 s5;
		uint64 s6;
		uint64 s7;
		uint64 s8;
		uint64 fp;
	};

	Context * proc_context_pool();

	// struct vma{
	// 	uint64 address; 	//start of mem block
	// 	int length;			//length of mem block
	// 	int next;			//next block	
	// };
}
//
// Created by Li Shuang ( pseudonym ) on 2024-06-24 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "kernel/types.hh"

#ifndef NUMCPU
#define NUMCPU 1
#endif

namespace hsai
{
	/*****************************************************************
	 * > HSAI 全局函数
	 * > 这个部分的函数由 HAL 来实现
	 *****************************************************************/

	void __hsai_hal hardware_abstract_init( void );

	void __hsai_hal hardware_secondary_init( void );


	

	class VirtualCpu;

	extern VirtualCpu * k_cpus[ NUMCPU ];
	VirtualCpu * get_cpu();

	class VirtualMemory;

	extern VirtualMemory * k_mem;

	class VirtualInterruptManager;

	extern VirtualInterruptManager * k_im;


} // namespace hsai

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
	void __hsai_hal hardware_abstract_init( void );

	void __hsai_hal hardware_secondary_init( void );

	void hsai_internal_init( void );
	

	// about CPU

	class VirtualCpu;

	extern VirtualCpu * k_cpus[ NUMCPU ];
	VirtualCpu * get_cpu();

	// about Memory
	
	class VirtualMemory;

	extern VirtualMemory * k_mem;

	// about interrupt
	
	class VirtualInterruptManager;

	extern VirtualInterruptManager * k_im;


	// about device management

	class DeviceManager;
	class ConsoleStdin;
	class ConsoleStdout;
	class ConsoleStderr;

	extern DeviceManager k_devm;

	extern ConsoleStdin k_stdin;
	extern ConsoleStdout k_stdout;
	extern ConsoleStderr k_stderr;

	// about architecture

	void * __hsai_hal get_ra();

} // namespace hsai

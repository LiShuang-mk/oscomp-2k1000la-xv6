//
// Created by Li Shuang ( pseudonym ) on 2024-07-10
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "hsai_global.hh"

#include "console.hh"
#include "device_manager.hh"
#include "hsai_log.hh"
#include "kernel/klib/__cxx_abi.hh"
#include "virtual_cpu.hh"

extern "C" {
extern ulong _bss_start_addr;
extern ulong _bss_end_addr;
}


namespace hsai
{
	VirtualCpu *k_cpus[NUMCPU] = { nullptr };

	VirtualCpu *get_cpu()
	{
		u32 cpuid = k_cpus[0]->get_cpu_id();
		return k_cpus[cpuid];
	}

	VirtualMemory *k_mem = nullptr;

	VirtualInterruptManager *k_im = nullptr;

	ConsoleStdin  k_stdin;
	ConsoleStdout k_stdout;
	ConsoleStderr k_stderr;

	void hsai_internal_init()
	{
		__cxxabiv1::__init_atexit_func_entry();

		new ( &k_devm ) DeviceManager;
		if ( k_devm.register_stdin( (VirtualDevice *) &k_stdin ) < 0 )
			while ( 1 );
		if ( k_devm.register_stdout( (VirtualDevice *) &k_stdout ) < 0 )
			while ( 1 );
		if ( k_devm.register_stderr( (VirtualDevice *) &k_stderr ) < 0 )
			while ( 1 );
	}

} // namespace hsai

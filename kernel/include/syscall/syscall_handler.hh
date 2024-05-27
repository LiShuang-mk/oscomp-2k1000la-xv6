//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "klib/function.hh"

#include <EASTL/string.h>
#include <EASTL/vector.h>


namespace fs
{
	struct xv6_file;
} // namespace fs


namespace syscall
{
	constexpr uint max_syscall_funcs_num = 2048;
	constexpr uint max_path_len = 128;
	constexpr uint max_arg_num = 32;

	class SyscallHandler
	{
	private:
		std::function<uint64( void )> _syscall_funcs[ max_syscall_funcs_num ];
		eastl::string _path;
		eastl::vector<eastl::string> _argv;


	public:
		SyscallHandler() {};
		void init();
		uint64 invoke_syscaller( uint64 sys_num );


	private:
		int _fetch_addr( uint64 addr, uint64 &out_data );
		int _fetch_str( uint64 addr, void *buf, uint64 max );
		int _fetch_str( uint64 addr, eastl::string &str, uint64 max );
		uint64 _arg_raw( int arg_n );
		int _arg_fd( int arg_n, int *out_fd, fs::xv6_file **out_f );

		int _arg_int( int arg_n, int &out_int ) { out_int = _arg_raw( arg_n ); return 0; }
		int _arg_addr( int arg_n, uint64 &out_addr ) { out_addr = _arg_raw( arg_n ); return 0; }
		int _arg_str( int arg_n, char *buf, int max )
		{
			uint64 addr;
			if ( _arg_addr( arg_n, addr ) < 0 )
				return -1;
			return _fetch_str( addr, buf, max );
		}
		int _arg_str( int arg_n, eastl::string &buf, int max )
		{
			uint64 addr;
			if ( _arg_addr( arg_n, addr ) < 0 )
				return -1;
			return _fetch_str( addr, buf, max );
		}


	private:	// ================ syscall functions ================
		uint64 _sys_write();
		uint64 _sys_fork();
		uint64 _sys_exit();
		uint64 _sys_getpid();
		uint64 _sys_getppid();
		uint64 _sys_brk();
		uint64 _sys_exec();
		uint64 _sys_wait();
		uint64 _sys_poweroff();
		uint64 _sys_dup();
		uint64 _sys_dup2();
		uint64 _sys_getcwd();
		uint64 _sys_gettimeofday();
		uint64 _sys_sched_yield();
		uint64 _sys_sleep();
		uint64 _sys_times();
		uint64 _sys_uname();
		uint64 _sys_openat();
	};

	extern SyscallHandler k_syscall_handler;




} // namespace syscall

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
	class File;
	class file;
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
		const char * _syscall_name[ max_syscall_funcs_num ];

	public:
		SyscallHandler() {};
		void init();
		uint64 invoke_syscaller( uint64 sys_num );


	private:
		int _fetch_addr( uint64 addr, uint64 &out_data );
		int _fetch_str( uint64 addr, void *buf, uint64 max );
		int _fetch_str( uint64 addr, eastl::string &str, uint64 max );
		uint64 _arg_raw( int arg_n );
		int _arg_fd( int arg_n, int *out_fd, fs::file **out_f );

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
		uint64 _sys_read();
		uint64 _sys_fork();
		uint64 _sys_exit();
		uint64 _sys_getpid();
		uint64 _sys_getppid();
		uint64 _sys_brk();
		uint64 _sys_execve();
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
		uint64 _sys_close();
		uint64 _sys_fstat();
		uint64 _sys_getdents();
		uint64 _sys_mkdir();
		uint64 _sys_chdir();
		uint64 _sys_mount();
		uint64 _sys_umount();
		uint64 _sys_mmap();
		uint64 _sys_munmap();
		uint64 _sys_statx();
		uint64 _sys_unlinkat();
		uint64 _sys_pipe();
		uint64 _sys_set_tid_address();
		uint64 _sys_set_robust_list();
		uint64 _sys_prlimit64();
		uint64 _sys_clock_gettime();
		uint64 _sys_mprotect();
		uint64 _sys_getuid();
		uint64 _sys_ioctl();
		uint64 _sys_readlinkat();
		uint64 _sys_getrandom();
		uint64 _sys_sigaction();
		uint64 _sys_fcntl();
		uint64 _sys_getpgid();
		uint64 _sys_setpgid();
		uint64 _sys_geteuid();
		uint64 _sys_ppoll();
		uint64 _sys_getgid();
		uint64 _sys_setgid();
		uint64 _sys_setuid();
		uint64 _sys_sendfile();
		uint64 _sys_exit_group();
		uint64 _sys_statfs();
		uint64 _sys_syslog();
		uint64 _sys_faccessat();
		uint64 _sys_sysinfo();
		uint64 _sys_nanosleep();
		uint64 _sys_getrusage();
		uint64 _sys_utimensat();
		uint64 _sys_lseek();
		uint64 _sys_splice();
		uint64 _sys_sigprocmask();
	};

	extern SyscallHandler k_syscall_handler;




} // namespace syscall

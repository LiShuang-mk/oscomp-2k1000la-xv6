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


namespace fs
{
	struct xv6_file;
} // namespace fs


namespace syscall
{
	constexpr uint max_syscall_funcs_num = 2048;

	class SyscallHandler
	{
	private:
		std::function<uint64( void )> _syscall_funcs[ max_syscall_funcs_num ];


	public:
		SyscallHandler() {};
		void init();
		uint64 invoke_syscaller( uint64 sys_num );


	private:
		int _fetch_addr( uint64 addr, uint64 &out_data );
		int _fetch_str( uint64 addr, void *buf, uint64 max );
		uint64 _arg_raw( int arg_n );

		int _arg_int( int arg_n, int &out_int ) { out_int = _arg_raw( arg_n ); return 0; }
		int _arg_addr( int arg_n, uint64 &out_addr ) { out_addr = _arg_raw( arg_n ); return 0; }
		int _arg_fd( int arg_n, int *out_fd, fs::xv6_file **out_f );

		uint64 test_bind();

	private:	// ================ syscall functions ================
		uint64 _sys_write();
	};

	extern SyscallHandler k_syscall_handler;

	
	

} // namespace syscall

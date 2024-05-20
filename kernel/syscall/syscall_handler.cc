//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "syscall/syscall_handler.hh"
#include "hal/cpu.hh"
#include "pm/process.hh"
#include "pm/trap_frame.hh"
#include "mm/virtual_memory_manager.hh"
#include "fs/file.hh"
#include "klib/klib.hh"

namespace syscall
{
	SyscallHandler k_syscall_handler;

	void SyscallHandler::init()
	{
		for ( auto &func : _syscall_funcs )
		{
			func = [] () -> uint64
			{
				printf( "defualt syscall\n" );
				return 0;
			};
		}
	}

	uint64 SyscallHandler::invoke_syscaller( uint64 sys_num )
	{
		return _syscall_funcs[ sys_num ]();
	}


// ---------------- private helper functions ----------------

	int SyscallHandler::_fetch_addr( uint64 addr, uint64 &out_data )
	{
		pm::Pcb *p = loongarch::Cpu::get_cpu()->get_cur_proc();
		if ( addr >= p->get_size() || addr + sizeof( uint64 ) > p->get_size() )
			return -1;
		mm::PageTable pt = p->get_pagetable();
		if ( mm::k_vmm.copy_in( pt, &out_data, addr, sizeof( out_data ) ) < 0 )
			return -1;
		return 0;
	}

	int SyscallHandler::_fetch_str( uint64 addr, void *buf, uint64 max )
	{
		pm::Pcb *p = loongarch::Cpu::get_cpu()->get_cur_proc();
		mm::PageTable pt = p->get_pagetable();
		int err = mm::k_vmm.copy_str_in( pt, buf, addr, max );
		if ( err < 0 )
			return err;
		return strlen( ( const char * ) buf );
	}

	uint64 SyscallHandler::_arg_raw( int arg_n )
	{
		pm::Pcb *p = loongarch::Cpu::get_cpu()->get_cur_proc();
		switch ( arg_n )
		{
			case 0:
				return p->get_trapframe()->a0;
			case 1:
				return p->get_trapframe()->a1;
			case 2:
				return p->get_trapframe()->a2;
			case 3:
				return p->get_trapframe()->a3;
			case 4:
				return p->get_trapframe()->a4;
			case 5:
				return p->get_trapframe()->a5;
		}
		log_panic( "arg raw" );
		return -1;
	}

	int SyscallHandler::_arg_fd( int arg_n, int *out_fd, fs::xv6_file **out_f )
	{
		int fd;
		fs::xv6_file *f;

		if ( _arg_int( arg_n, fd ) < 0 )
			return -1;
		pm::Pcb *p = loongarch::Cpu::get_cpu()->get_cur_proc();
		f = p->get_open_file( fd );
		if ( f == nullptr )
			return -1;
		if ( out_fd )
			*out_fd = fd;
		if ( out_f )
			*out_f = f;

		return 0;
	}


} // namespace syscall

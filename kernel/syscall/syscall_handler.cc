//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "syscall/syscall_handler.hh"
#include "syscall/syscall_defs.hh"
#include "hal/cpu.hh"
#include "fs/file.hh"
#include "fs/dev/acpi_controller.hh"
#include "pm/process.hh"
#include "pm/trap_frame.hh"
#include "pm/process_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "tm/timer_manager.hh"
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
		_argv.clear();
		_path.clear();

		_syscall_funcs[ SYS_write ] = std::bind( &SyscallHandler::_sys_write, this );
		_syscall_funcs[ SYS_exit ] = std::bind( &SyscallHandler::_sys_exit, this );
		_syscall_funcs[ SYS_fork ] = std::bind( &SyscallHandler::_sys_fork, this );
		_syscall_funcs[ SYS_getpid ] = std::bind( &SyscallHandler::_sys_getpid, this );
		_syscall_funcs[ SYS_getppid ] = std::bind( &SyscallHandler::_sys_getppid, this );
		_syscall_funcs[ SYS_brk ] = std::bind( &SyscallHandler::_sys_brk, this );
		_syscall_funcs[ SYS_exec ] = std::bind( &SyscallHandler::_sys_exec, this );
		_syscall_funcs[ SYS_wait ] = std::bind( &SyscallHandler::_sys_wait, this );
		_syscall_funcs[ SYS_poweroff ] = std::bind( &SyscallHandler::_sys_poweroff, this );
		_syscall_funcs[ SYS_dup ] = std::bind( &SyscallHandler::_sys_dup, this );
		_syscall_funcs[ SYS_dup2 ] = std::bind( &SyscallHandler::_sys_dup2, this );
		_syscall_funcs[ SYS_getcwd ] = std::bind( &SyscallHandler::_sys_getcwd, this );
		_syscall_funcs[ SYS_gettimeofday ] = std::bind( &SyscallHandler::_sys_gettimeofday, this );
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

	int SyscallHandler::_fetch_str( uint64 addr, eastl::string &str, uint64 max )
	{
		pm::Pcb *p = loongarch::Cpu::get_cpu()->get_cur_proc();
		mm::PageTable pt = p->get_pagetable();
		int err = mm::k_vmm.copy_str_in( pt, str, addr, max );
		if ( err < 0 )
			return err;
		return str.size();
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

// ---------------- syscall functions ----------------

	uint64 SyscallHandler::_sys_write()
	{
		fs::xv6_file *f;
		int n;
		uint64 p;

		if ( _arg_fd( 0, nullptr, &f ) < 0
			|| _arg_addr( 1, p ) < 0
			|| _arg_int( 2, n ) < 0 )
		{
			return -1;
		}

		return f->write( p, n );
	}

	uint64 SyscallHandler::_sys_exit()
	{
		int n;
		if ( _arg_int( 0, n ) < 0 )
			return -1;
		pm::k_pm.exit( n );
		return 0;  // not reached
	}

	uint64 SyscallHandler::_sys_fork()
	{
		return pm::k_pm.fork();
	}

	uint64 SyscallHandler::_sys_getpid()
	{
		return pm::k_pm.get_cur_pcb()->get_pid();
	}

	uint64 SyscallHandler::_sys_getppid()
	{
		return pm::k_pm.get_cur_pcb()->get_ppid();
	}

	uint64 SyscallHandler::_sys_brk()
	{
		int n;
		if ( _arg_int( 0, n ) < 0 )
			return -1;
		return pm::k_pm.brk( n );
	}

	uint64 SyscallHandler::_sys_exec()
	{
		uint i;
		uint64 uargv, uarg;

		_path.clear();
		if ( _arg_str( 0, _path, mm::pg_size ) < 0 || _arg_addr( 1, uargv ) < 0 )
		{
			return -1;
		}
		_argv.clear();

		bool is_bad = false;
		for ( i = 0; ; i++ )
		{
			if ( i >= max_arg_num )
			{
				is_bad = true;
				break;
			}
			if ( _fetch_addr( uargv + sizeof( uint64 ) * i, uarg ) < 0 )
			{
				is_bad = true;
				break;
			}
			if ( uarg == 0 )
			{
				_argv[ i ].clear();
				break;
			}
			_argv.emplace_back( eastl::string() );
			if ( _fetch_str( uarg, _argv[ i ], mm::pg_size ) < 0 )
			{
				is_bad = true;
				break;
			}
		}

		int ret;
		if ( is_bad )
		{
			ret = -1;
		}
		else
		{
			_argv.clear();
			ret = pm::k_pm.exec( _path, _argv );
		}

		_argv.clear();
		return ret;
	}

	uint64 SyscallHandler::_sys_wait()
	{
		uint64 p;
		if ( _arg_addr( 1, p ) < 0 )
			return -1;
		return pm::k_pm.wait( p );
	}

	uint64 SyscallHandler::_sys_poweroff()
	{
		dev::acpi::k_acpi_controller.power_off();
		return 0;
	}

	uint64 SyscallHandler::_sys_dup()
	{
		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		fs::xv6_file * f;
		int fd;

		if ( _arg_fd( 0, nullptr, &f ) < 0 )
			return -1;
		if ( ( fd = pm::k_pm.alloc_fd( p, f ) ) < 0 )
			return -1;
		fs::k_file_table.dup( f );
		return fd;
	}

	uint64 SyscallHandler::_sys_dup2()
	{
		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		fs::xv6_file * f;
		int fd;

		if ( _arg_fd( 0, nullptr, &f ) < 0 )
			return -1;
		if ( _arg_int( 1, fd ) < 0 )
			return -1;
		if ( pm::k_pm.alloc_fd( p, f, fd ) < 0 )
			return -1;
		fs::k_file_table.dup( f );
		return fd;
	}

	uint64 SyscallHandler::_sys_getcwd()
	{
		char cwd[] = "/";
		uint64 buf;
		int size;

		if ( _arg_addr( 0, buf ) < 0 )
			return -1;
		if ( _arg_int( 1, size ) < 0 )
			return -1;
		if ( size < ( int ) sizeof( cwd ) )
			return -1;

		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable pt = p->get_pagetable();
		if ( mm::k_vmm.copyout( pt, buf, ( const void * ) cwd, sizeof( cwd ) ) < 0 )
			return -1;

		return buf;
	}

	uint64 SyscallHandler::_sys_gettimeofday()
	{
		uint64 tv_addr;
		tmm::timeval tv;

		if ( _arg_addr( 0, tv_addr ) < 0 )
			return -1;
		
		tv = tmm::k_tm.get_time_val();
		
		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable pt = p->get_pagetable();
		if ( mm::k_vmm.copyout( pt, tv_addr, ( const void * ) &tv, sizeof( tv ) ) < 0 )
			return -1;

		return 0;
	}

} // namespace syscall

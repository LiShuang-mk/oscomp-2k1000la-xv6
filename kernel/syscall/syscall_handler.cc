//
// Created by Li Shuang ( pseudonym ) on 2024-05-20
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "syscall/syscall_handler.hh"

// include syscall number
#include "syscall/syscall_defs.hh"
//

#include <EASTL/random.h>
#include <asm-generic/poll.h>
#include <linux/sysinfo.h>
#include <sys/ioctl.h>
// #include <sys/resource.h>

#include <hsai_global.hh>
#include <mem/virtual_memory.hh>
#include <process_interface.hh>

#include "fs/dev/acpi_controller.hh"
#include "fs/file/device.hh"
#include "fs/file/file.hh"
#include "fs/file/normal.hh"
#include "fs/file/pipe.hh"
#include "fs/kstat.hh"
#include "fs/path.hh"
#include "hal/cpu.hh"
#include "klib/klib.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/userspace_stream.hh"
#include "mm/userstack_stream.hh"
#include "mm/virtual_memory_manager.hh"
#include "pm/futex.hh"
#include "pm/ipc/signal.hh"
#include "pm/prlimit.hh"
#include "pm/process.hh"
#include "pm/process_manager.hh"
#include "pm/scheduler.hh"
#include "tm/time.hh"
#include "tm/timer_manager.hh"
namespace syscall
{
	SyscallHandler k_syscall_handler;

#define BIND_SYSCALL( sys_name )                                                          \
	_syscall_funcs[SYS_##sys_name] = std::bind( &SyscallHandler::_sys_##sys_name, this ); \
	_syscall_name[SYS_##sys_name]  = #sys_name

	void SyscallHandler::init()
	{
		for ( auto &func : _syscall_funcs )
		{
			func = []() -> uint64
			{
				printf( "defualt syscall\n" );
				return 0;
			};
		}
		for ( auto &n : _syscall_name ) n = nullptr;

		BIND_SYSCALL( write );
		BIND_SYSCALL( read );
		BIND_SYSCALL( exit );
		BIND_SYSCALL( fork );
		BIND_SYSCALL( getpid );
		BIND_SYSCALL( getppid );
		BIND_SYSCALL( brk );
		BIND_SYSCALL( execve );
		BIND_SYSCALL( wait );
		BIND_SYSCALL( poweroff );
		BIND_SYSCALL( dup );
		BIND_SYSCALL( dup2 );
		BIND_SYSCALL( getcwd );
		BIND_SYSCALL( gettimeofday );
		BIND_SYSCALL( sched_yield );
		BIND_SYSCALL( sleep );
		BIND_SYSCALL( times );
		BIND_SYSCALL( uname );
		BIND_SYSCALL( openat );
		BIND_SYSCALL( close );
		BIND_SYSCALL( fstat );
		BIND_SYSCALL( getdents );
		BIND_SYSCALL( mkdir );
		BIND_SYSCALL( chdir );
		BIND_SYSCALL( mount );
		BIND_SYSCALL( umount );
		BIND_SYSCALL( mmap );
		BIND_SYSCALL( munmap );
		BIND_SYSCALL( statx );
		BIND_SYSCALL( unlinkat );
		BIND_SYSCALL( pipe );
		BIND_SYSCALL( set_tid_address );
		BIND_SYSCALL( set_robust_list );
		BIND_SYSCALL( prlimit64 );
		BIND_SYSCALL( clock_gettime );
		BIND_SYSCALL( mprotect );
		BIND_SYSCALL( getuid );
		BIND_SYSCALL( readlinkat );
		BIND_SYSCALL( getrandom );
		BIND_SYSCALL( sigaction );
		BIND_SYSCALL( ioctl );
		BIND_SYSCALL( fcntl );
		BIND_SYSCALL( getpgid );
		BIND_SYSCALL( setpgid );
		BIND_SYSCALL( geteuid );
		BIND_SYSCALL( ppoll );
		BIND_SYSCALL( getgid );
		BIND_SYSCALL( setgid );
		BIND_SYSCALL( setuid );
		BIND_SYSCALL( sendfile );
		BIND_SYSCALL( exit_group );
		BIND_SYSCALL( statfs );
		BIND_SYSCALL( syslog );
		BIND_SYSCALL( faccessat );
		BIND_SYSCALL( sysinfo );
		BIND_SYSCALL( nanosleep );
		BIND_SYSCALL( getrusage );
	}

	uint64 SyscallHandler::invoke_syscaller( uint64 sys_num )
	{
#ifdef OS_DEBUG
		if ( sys_num != SYS_write )
		{
			if ( _syscall_name[sys_num] != nullptr )
				printf( YELLOW_COLOR_PRINT "syscall %16s", _syscall_name[sys_num] );
			else
				printf( BRIGHT_YELLOW_COLOR_PRINT "unknown sycall %d\t", sys_num );
			auto [usg, rst] = mm::k_pmm.mem_desc();
			printf( "mem-usage: %_-10ld mem-rest: %_-10ld\n" CLEAR_COLOR_PRINT, usg, rst );
		}
#endif
		return _syscall_funcs[sys_num]();
	}


	// ---------------- private helper functions ----------------

	int SyscallHandler::_fetch_addr( uint64 addr, uint64 &out_data )
	{
		pm::Pcb		  *p  = (pm::Pcb *) hsai::get_cur_proc();
		// if ( addr >= p->get_size() || addr + sizeof( uint64 ) > p->get_size()
		// ) 	return -1;
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_in( *pt, &out_data, addr, sizeof( out_data ) ) < 0 ) return -1;
		return 0;
	}

	int SyscallHandler::_fetch_str( uint64 addr, void *buf, uint64 max )
	{
		pm::Pcb		  *p   = (pm::Pcb *) hsai::get_cur_proc();
		mm::PageTable *pt  = p->get_pagetable();
		int			   err = mm::k_vmm.copy_str_in( *pt, buf, addr, max );
		if ( err < 0 ) return err;
		return strlen( (const char *) buf );
	}

	int SyscallHandler::_fetch_str( uint64 addr, eastl::string &str, uint64 max )
	{
		pm::Pcb		  *p   = (pm::Pcb *) hsai::get_cur_proc();
		mm::PageTable *pt  = p->get_pagetable();
		int			   err = mm::k_vmm.copy_str_in( *pt, str, addr, max );
		if ( err < 0 ) return err;
		return str.size();
	}

	uint64 SyscallHandler::_arg_raw( int arg_n )
	{
		return hsai::get_arg_from_trap_frame(
			hsai::get_trap_frame_from_proc( hsai::get_cur_proc() ), (uint) arg_n );
	}

	int SyscallHandler::_arg_fd( int arg_n, int *out_fd, fs::file **out_f )
	{
		int		  fd;
		fs::file *f;

		if ( _arg_int( arg_n, fd ) < 0 ) return -1;
		pm::Pcb *p = (pm::Pcb *) hsai::get_cur_proc();
		f		   = p->get_open_file( fd );
		if ( f == nullptr ) return -1;
		if ( out_fd ) *out_fd = fd;
		if ( out_f ) *out_f = f;

		return 0;
	}

	// ---------------- syscall functions ----------------

	uint64 SyscallHandler::_sys_write()
	{
		fs::file			*f;
		int					 n;
		uint64				 p;
		[[maybe_unused]] int fd = 0;

		if ( _arg_fd( 0, &fd, &f ) < 0 || _arg_addr( 1, p ) < 0 || _arg_int( 2, n ) < 0 )
		{
			return -1;
		}

		if ( fd > 2 ) printf( BLUE_COLOR_PRINT "invoke sys_write\n" CLEAR_COLOR_PRINT );

		pm::Pcb		  *proc = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt	= proc->get_pagetable();

		char *buf = new char[n + 10];
		{
			mm::UserspaceStream uspace( (void *) p, n + 1, pt );
			uspace.open();
			mm::UsRangeDesc urd = std::make_tuple( (u8 *) buf, (ulong) n + 1 );
			uspace >> urd;
			uspace.close();
		}
		// if ( mm::k_vmm.copy_in( *pt, (void *) buf, p, n ) < 0 ) return -1;

		// if ( buf[0] == '#' && buf[1] == '#' && buf[2] == '#' && buf[3] == '#'
		// )
		// {
		// 	printf( YELLOW_COLOR_PRINT "note : echo ####\n" CLEAR_COLOR_PRINT );
		// }

		long rc = f->write( (ulong) buf, n, f->get_file_offset(), true );
		delete[] buf;
		return rc;
	}

	uint64 SyscallHandler::_sys_read()
	{
		fs::file			*f;
		uint64				 buf;
		int					 n;
		[[maybe_unused]] int fd = -1;

		if ( _arg_fd( 0, &fd, &f ) < 0 ) return -1;
		if ( _arg_addr( 1, buf ) < 0 ) return -2;
		if ( _arg_int( 2, n ) < 0 ) return -3;

		if ( f == nullptr ) return -4;
		if ( n <= 0 ) return -5;

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();

		char *k_buf = new char[n + 1];
		int	  ret	= f->read( (uint64) k_buf, n, f->get_file_offset(), true );
		if ( ret < 0 ) return -6;
		if ( mm::k_vmm.copyout( *pt, buf, k_buf, ret ) < 0 ) return -7;

		delete[] k_buf;
		return ret;
	}

	uint64 SyscallHandler::_sys_exit()
	{
		int n;
		if ( _arg_int( 0, n ) < 0 ) return -1;
		pm::k_pm.exit( n );
		return 0; // not reached
	}

	uint64 SyscallHandler::_sys_fork()
	{
		uint64 u_sp;
		if ( _arg_addr( 1, u_sp ) < 0 ) return -1;

		return pm::k_pm.fork( u_sp );
	}

	uint64 SyscallHandler::_sys_getpid() { return pm::k_pm.get_cur_pcb()->get_pid(); }

	uint64 SyscallHandler::_sys_getppid() { return pm::k_pm.get_cur_pcb()->get_ppid(); }

	uint64 SyscallHandler::_sys_brk()
	{
		ulong n;
		if ( _arg_addr( 0, n ) < 0 ) return -1;
		return pm::k_pm.brk( n );
	}

	uint64 SyscallHandler::_sys_execve()
	{
		uint64 uargv, uenvp;

		eastl::string path;
		if ( _arg_str( 0, path, hsai::page_size ) < 0 || _arg_addr( 1, uargv ) < 0 ||
			 _arg_addr( 2, uenvp ) < 0 )
			return -1;

		log_trace( "execve fetch argv=%p", uargv );

		eastl::vector<eastl::string> argv;
		ulong						 uarg;
		if ( uargv != 0 )
		{
			for ( ulong i = 0, puarg = uargv;; i++, puarg += sizeof( char * ) )
			{
				if ( i >= max_arg_num ) return -1;

				if ( _fetch_addr( puarg, uarg ) < 0 ) return -1;

				if ( uarg == 0 ) break;

				log_trace( "execve get arga[%d] = %p", i, uarg );

				argv.emplace_back( eastl::string() );
				if ( _fetch_str( uarg, argv[i], hsai::page_size ) < 0 ) return -1;
			}
		}

		eastl::vector<eastl::string> envp;
		ulong						 uenv;
		if ( uenvp != 0 )
		{
			for ( ulong i = 0, puenv = uenvp;; i++, puenv += sizeof( char * ) )
			{
				if ( i >= max_arg_num ) return -2;

				if ( _fetch_addr( puenv, uenv ) < 0 ) return -2;

				if ( uenv == 0 ) break;

				envp.emplace_back( eastl::string() );
				if ( _fetch_str( uenv, envp[i], hsai::page_size ) < 0 ) return -2;
			}
		}

		return pm::k_pm.execve( path, argv, envp );
	}

	uint64 SyscallHandler::_sys_wait()
	{
		int	   pid;
		uint64 wstatus_addr;
		if ( _arg_int( 0, pid ) < 0 ) return -1;
		if ( _arg_addr( 1, wstatus_addr ) < 0 ) return -1;
		return pm::k_pm.wait( pid, wstatus_addr );
	}

	uint64 SyscallHandler::_sys_poweroff()
	{
		dev::acpi::k_acpi_controller.power_off();
		return 0;
	}

	uint64 SyscallHandler::_sys_dup()
	{
		pm::Pcb				*p = pm::k_pm.get_cur_pcb();
		fs::file			*f;
		int					 fd;
		[[maybe_unused]] int oldfd = 0;

		if ( _arg_fd( 0, &oldfd, &f ) < 0 ) return -1;
		if ( ( fd = pm::k_pm.alloc_fd( p, f ) ) < 0 ) return -1;
		// fs::k_file_table.dup( f );
		f->dup();
		return fd;
	}

	uint64 SyscallHandler::_sys_dup2()
	{
		pm::Pcb				*p = pm::k_pm.get_cur_pcb();
		fs::file			*f;
		int					 fd;
		[[maybe_unused]] int oldfd = 0;

		if ( _arg_fd( 0, &oldfd, &f ) < 0 ) return -1;
		if ( _arg_int( 1, fd ) < 0 ) return -1;
		if ( pm::k_pm.alloc_fd( p, f, fd ) < 0 ) return -1;
		// fs::k_file_table.dup( f );
		f->dup();
		return fd;
	}

	uint64 SyscallHandler::_sys_getcwd()
	{
		char   cwd[256];
		uint64 buf;
		int	   size;

		if ( _arg_addr( 0, buf ) < 0 ) return -1;
		if ( _arg_int( 1, size ) < 0 ) return -1;
		if ( size >= (int) sizeof( cwd ) ) return -1;

		pm::Pcb		  *p   = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt  = p->get_pagetable();
		uint		   len = pm::k_pm.getcwd( cwd );
		if ( mm::k_vmm.copyout( *pt, buf, (const void *) cwd, len ) < 0 ) return -1;

		return buf;
	}

	uint64 SyscallHandler::_sys_gettimeofday()
	{
		uint64		 tv_addr;
		tmm::timeval tv;

		if ( _arg_addr( 0, tv_addr ) < 0 ) return -1;

		tv = tmm::k_tm.get_time_val();

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copyout( *pt, tv_addr, (const void *) &tv, sizeof( tv ) ) < 0 ) return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_sched_yield()
	{
		pm::k_scheduler.yield();
		return 0;
	}

	uint64 SyscallHandler::_sys_sleep()
	{
		tmm::timeval tv;
		uint64		 tv_addr;

		if ( _arg_addr( 0, tv_addr ) < 0 ) return -1;

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_in( *pt, &tv, tv_addr, sizeof( tv ) ) < 0 ) return -1;

		return tmm::k_tm.sleep_from_tv( tv );
	}

	uint64 SyscallHandler::_sys_times()
	{
		tmm::tms tms_val;
		uint64	 tms_addr;

		if ( _arg_addr( 0, tms_addr ) < 0 ) return -1;

		pm::k_pm.get_cur_proc_tms( &tms_val );

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copyout( *pt, tms_addr, &tms_val, sizeof( tms_val ) ) < 0 ) return -1;

		return tmm::k_tm.get_ticks();
	}

	struct _Utsname
	{
		char sysname[65];
		char nodename[65];
		char release[65];
		char version[65];
		char machine[65];
		char domainname[65];
	};
	static const char _SYSINFO_sysname[]	= "XN6";
	static const char _SYSINFO_nodename[]	= "(none-node)";
	static const char _SYSINFO_release[]	= "V1.0";
	static const char _SYSINFO_version[]	= "V1.0";
	static const char _SYSINFO_machine[]	= "LoongArch-2k1000";
	static const char _SYSINFO_domainname[] = "(none-domain)";

	uint64 SyscallHandler::_sys_uname()
	{
		uint64 usta;
		uint64 sysa, noda, rlsa, vsna, mcha, dmna;

		if ( _arg_addr( 0, usta ) < 0 ) return -11;
		sysa = (uint64) ( ( (_Utsname *) usta )->sysname );
		noda = (uint64) ( ( (_Utsname *) usta )->nodename );
		rlsa = (uint64) ( ( (_Utsname *) usta )->release );
		vsna = (uint64) ( ( (_Utsname *) usta )->version );
		mcha = (uint64) ( ( (_Utsname *) usta )->machine );
		dmna = (uint64) ( ( (_Utsname *) usta )->domainname );


		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();

		if ( mm::k_vmm.copyout( *pt, sysa, _SYSINFO_sysname, sizeof( _SYSINFO_sysname ) ) < 0 )
			return -1;
		if ( mm::k_vmm.copyout( *pt, noda, _SYSINFO_nodename, sizeof( _SYSINFO_nodename ) ) < 0 )
			return -1;
		if ( mm::k_vmm.copyout( *pt, rlsa, _SYSINFO_release, sizeof( _SYSINFO_release ) ) < 0 )
			return -1;
		if ( mm::k_vmm.copyout( *pt, vsna, _SYSINFO_version, sizeof( _SYSINFO_version ) ) < 0 )
			return -1;
		if ( mm::k_vmm.copyout( *pt, mcha, _SYSINFO_machine, sizeof( _SYSINFO_machine ) ) < 0 )
			return -1;
		if ( mm::k_vmm.copyout( *pt, dmna, _SYSINFO_domainname, sizeof( _SYSINFO_domainname ) ) <
			 0 )
			return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_openat()
	{
		int	   dir_fd;
		uint64 path_addr;
		int	   flags;

		if ( _arg_int( 0, dir_fd ) < 0 ) return -1;
		if ( _arg_addr( 1, path_addr ) < 0 ) return -1;
		if ( _arg_int( 2, flags ) < 0 ) return -1;

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		eastl::string  path;
		if ( mm::k_vmm.copy_str_in( *pt, path, path_addr, 100 ) < 0 ) return -1;

		int res = pm::k_pm.open( dir_fd, path, flags );
		log_trace( "openat return fd is %d", res );
		return res;
	}

	uint64 SyscallHandler::_sys_close()
	{
		int fd;

		if ( _arg_int( 0, fd ) < 0 ) return -1;
		return pm::k_pm.close( fd );
	}

	uint64 SyscallHandler::_sys_fstat()
	{
		int		  fd;
		fs::Kstat kst;
		uint64	  kst_addr;

		if ( _arg_int( 0, fd ) < 0 ) return -1;

		if ( _arg_addr( 1, kst_addr ) < 0 ) return -1;

		pm::k_pm.fstat( fd, &kst );
		mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
		if ( mm::k_vmm.copyout( *pt, kst_addr, &kst, sizeof( kst ) ) < 0 ) return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_getdents()
	{
		fs::file *f;
		uint64	  buf_addr;
		uint64	  buf_len;

		if ( _arg_fd( 0, nullptr, &f ) < 0 ) return -1;
		if ( _arg_addr( 1, buf_addr ) < 0 ) return -1;
		if ( _arg_addr( 2, buf_len ) < 0 ) return -1;

		if ( f->_attrs.filetype != fs::FileTypes::FT_NORMAL &&
			 f->_attrs.filetype != fs::FileTypes::FT_DIRECT )
			return -1;
		// eastl::string name = f->data.get_Entry()->rName();
		fs::normal_file *normal_f = static_cast<fs::normal_file *>( f );

		mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();

		mm::UserspaceStream us( (void *) buf_addr, buf_len, pt );

		us.open();
		u64 rlen = us.rest_space();
		normal_f->read_sub_dir( us );
		rlen -= us.rest_space();
		us.close();

		return rlen;
	}

	uint64 SyscallHandler::_sys_mkdir() { return 0; }

	uint64 SyscallHandler::_sys_chdir()
	{
		eastl::string path;

		if ( _arg_str( 0, path, hsai::page_size ) < 0 ) return -1;

		return pm::k_pm.chdir( path );
	}

	uint64 SyscallHandler::_sys_mount()
	{
		uint64		   dev_addr;
		uint64		   mnt_addr;
		uint64		   fstype_addr;
		eastl::string  dev;
		eastl::string  mnt;
		eastl::string  fstype;
		int			   flags;
		uint64		   data;
		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();

		if ( _arg_addr( 0, dev_addr ) < 0 ) return -1;
		if ( _arg_addr( 1, mnt_addr ) < 0 ) return -1;
		if ( _arg_addr( 2, fstype_addr ) < 0 ) return -1;

		if ( mm::k_vmm.copy_str_in( *pt, dev, dev_addr, 100 ) < 0 ) return -1;
		if ( mm::k_vmm.copy_str_in( *pt, mnt, mnt_addr, 100 ) < 0 ) return -1;
		if ( mm::k_vmm.copy_str_in( *pt, fstype, fstype_addr, 100 ) < 0 ) return -1;

		if ( _arg_int( 3, flags ) < 0 ) return -1;
		if ( _arg_addr( 4, data ) < 0 ) return -1;

		// return pm::k_pm.mount( dev, mnt, fstype, flags, data );
		fs::Path devpath( dev );
		fs::Path mntpath( mnt );

		return devpath.mount( mntpath, fstype, flags, data );
	}

	uint64 SyscallHandler::_sys_umount()
	{
		uint64		  specialaddr;
		eastl::string special;
		int			  flags;

		pm::Pcb		  *cur = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt  = cur->get_pagetable();

		if ( _arg_addr( 0, specialaddr ) < 0 ) return -1;
		if ( _arg_int( 1, flags ) < 0 ) return -1;

		if ( mm::k_vmm.copy_str_in( *pt, special, specialaddr, 100 ) < 0 ) return -1;

		fs::Path specialpath( special );
		return specialpath.umount( flags );
	}

	uint64 SyscallHandler::_sys_mmap()
	{
		u64 addr;
		if ( _arg_addr( 0, addr ) < 0 ) return -1;

		size_t map_size;
		if ( _arg_addr( 1, map_size ) < 0 ) return -1;

		int prot;
		if ( _arg_int( 2, prot ) < 0 ) return -1;

		int flags;
		if ( _arg_int( 3, flags ) < 0 ) return -1;

		int fd;
		if ( _arg_int( 4, fd ) < 0 ) return -1;

		size_t length;
		if ( _arg_addr( 5, length ) < 0 ) return -1;

		return pm::k_pm.mmap( fd, map_size );
	}

	uint64 SyscallHandler::_sys_munmap() { return 0; }

	uint64 SyscallHandler::_sys_statx()
	{
		using __u16 = uint16;
		using __u32 = uint32;
		using __s64 = int64;
		using __u64 = uint64;


		struct statx_timestamp
		{
			__s64 tv_sec;  /* Seconds since the Epoch (UNIX time) */
			__u32 tv_nsec; /* Nanoseconds since tv_sec */
		};
		struct statx
		{
			__u32 stx_mask;		  /* Mask of bits indicating
									 filled fields */
			__u32 stx_blksize;	  /* Block size for filesystem I/O */
			__u64 stx_attributes; /* Extra file attribute indicators */
			__u32 stx_nlink;	  /* Number of hard links */
			__u32 stx_uid;		  /* User ID of owner */
			__u32 stx_gid;		  /* Group ID of owner */
			__u16 stx_mode;		  /* File type and mode */
			__u64 stx_ino;		  /* Inode number */
			__u64 stx_size;		  /* Total size in bytes */
			__u64 stx_blocks;	  /* Number of 512B blocks allocated */
			__u64 stx_attributes_mask;
			/* Mask to show what's supported
			   in stx_attributes */

			/* The following fields are file timestamps */
			struct statx_timestamp stx_atime; /* Last access */
			struct statx_timestamp stx_btime; /* Creation */
			struct statx_timestamp stx_ctime; /* Last status change */
			struct statx_timestamp stx_mtime; /* Last modification */

			/* If this file represents a device, then the next two
			   fields contain the ID of the device */
			__u32 stx_rdev_major; /* Major ID */
			__u32 stx_rdev_minor; /* Minor ID */

			/* The next two fields contain the ID of the device
			   containing the filesystem where the file resides */
			__u32 stx_dev_major; /* Major ID */
			__u32 stx_dev_minor; /* Minor ID */

			__u64 stx_mnt_id; /* Mount ID */

			/* Direct I/O alignment restrictions */
			__u32 stx_dio_mem_align;
			__u32 stx_dio_offset_align;
		};

		int			  fd;
		eastl::string path_name;
		fs::Kstat	  kst;
		statx		  stx;
		uint64		  kst_addr;

		if ( _arg_int( 0, fd ) < 0 ) return -1;

		if ( _arg_str( 1, path_name, 128 ) < 0 ) return -1;

		if ( _arg_addr( 4, kst_addr ) < 0 ) return -1;

		if ( fd > 0 )
		{
			pm::k_pm.fstat( fd, &kst );
			stx.stx_mode	  = kst.mode;
			stx.stx_size	  = kst.size;
			mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
			if ( mm::k_vmm.copyout( *pt, kst_addr, &stx, sizeof( stx ) ) < 0 ) return -1;
			return 0;
		}
		else
		{
			int ffd;
			ffd = pm::k_pm.open( fd, path_name, 2 );
			if ( ffd < 0 ) return -1;
			pm::k_pm.fstat( ffd, &kst );
			pm::k_pm.close( ffd );
			stx.stx_size	  = kst.size;
			stx.stx_mode	  = kst.mode;
			mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
			if ( mm::k_vmm.copyout( *pt, kst_addr, &stx, sizeof( stx ) ) < 0 ) return -1;
			return 0;
		}
	}

	uint64 SyscallHandler::_sys_unlinkat()
	{
		int	   fd, flags;
		uint64 path_addr;

		if ( _arg_int( 0, fd ) < 0 ) return -1;
		if ( _arg_addr( 1, path_addr ) < 0 ) return -1;
		if ( _arg_int( 2, flags ) < 0 ) return -1;
		eastl::string  path;
		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_str_in( *pt, path, path_addr, 100 ) < 0 ) return -1;

		int res = pm::k_pm.unlink( fd, path, flags );
		return res;
	}

	uint64 SyscallHandler::_sys_pipe()
	{
		int	   fd[2];
		uint64 addr;

		if ( _arg_addr( 0, addr ) < 0 ) return -1;

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_in( *pt, &fd, addr, 2 * sizeof( fd[0] ) ) < 0 ) return -1;

		if ( pm::k_pm.pipe( fd, 0 ) < 0 ) return -1;

		if ( mm::k_vmm.copyout( *pt, addr, &fd, 2 * sizeof( fd[0] ) ) < 0 ) return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_set_tid_address()
	{
		ulong addr;
		int	 *tidptr;

		if ( _arg_addr( 0, addr ) < 0 ) return -1;

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		tidptr			  = (int *) hsai::k_mem->to_vir( pt->walk_addr( addr ) );
		if ( tidptr == nullptr ) return -10;

		return pm::k_pm.set_tid_address( tidptr );
	}

	uint64 SyscallHandler::_sys_set_robust_list()
	{
		ulong				  addr;
		pm::robust_list_head *head;
		size_t				  len;

		if ( _arg_addr( 0, addr ) < 0 ) return -1;

		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		head			  = (pm::robust_list_head *) hsai::k_mem->to_vir( pt->walk_addr( addr ) );

		if ( head == nullptr ) return -10;

		if ( _arg_addr( 1, len ) < 0 ) return -2;

		return pm::k_pm.set_robust_list( head, len );
	}

	uint64 SyscallHandler::_sys_prlimit64()
	{
		int pid;
		if ( _arg_int( 0, pid ) < 0 ) return -1;

		int rsrc;
		if ( _arg_int( 1, rsrc ) < 0 ) return -2;

		u64 new_limit;
		u64 old_limit;
		if ( _arg_addr( 2, new_limit ) < 0 ) return -3;
		if ( _arg_addr( 3, old_limit ) < 0 ) return -4;

		pm::rlimit64  *nlim = nullptr, *olim = nullptr;
		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( new_limit != 0 )
			nlim = (pm::rlimit64 *) hsai::k_mem->to_vir( pt->walk_addr( new_limit ) );
		if ( old_limit != 0 )
			olim = (pm::rlimit64 *) hsai::k_mem->to_vir( pt->walk_addr( old_limit ) );

		return pm::k_pm.prlimit64( pid, rsrc, nlim, olim );
	}

	uint64 SyscallHandler::_sys_clock_gettime()
	{
		int clock_id;
		if ( _arg_int( 0, clock_id ) < 0 ) return -1;

		u64 addr;
		if ( _arg_addr( 1, addr ) < 0 ) return -2;

		tmm::timespec *tp = nullptr;
		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( addr != 0 ) tp = (tmm::timespec *) hsai::k_mem->to_vir( pt->walk_addr( addr ) );

		tmm::SystemClockId cid = (tmm::SystemClockId) clock_id;

		return tmm::k_tm.clock_gettime( cid, tp );
	}

	uint64 SyscallHandler::_sys_mprotect() { return 0; }

	uint64 SyscallHandler::_sys_getuid() { return 1; }

	uint64 SyscallHandler::_sys_readlinkat()
	{
		pm::Pcb		  *p  = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		int			   fd;
		fs::Path	   filePath;
		size_t		   ret;

		if ( _arg_int( 0, fd ) < 0 ) return -1;

		eastl::string path;
		if ( _arg_str( 1, path, 256 ) < 0 ) return -1;

		uint64 buf;
		if ( _arg_addr( 2, buf ) < 0 ) return -1;

		size_t buf_size;
		if ( _arg_addr( 3, buf_size ) < 0 ) return -1;

		if ( fd == AT_FDCWD )
			new ( &filePath ) fs::Path( path, p->_cwd );
		else
			new ( &filePath ) fs::Path( path, p->_ofile[fd] );

		fs::dentry *dent = filePath.pathSearch();
		if ( dent == nullptr ) return -1;

		char *buffer = new char[buf_size];
		ret			 = dent->getNode()->readlinkat( buffer, buf_size );

		if ( mm::k_vmm.copyout( *pt, buf, (void *) buffer, ret ) < 0 )
		{
			delete[] buffer;
			return -1;
		}

		delete[] buffer;
		return ret;
	}

	uint64 SyscallHandler::_sys_getrandom()
	{
		uint64				 bufaddr;
		int					 buflen;
		[[maybe_unused]] int flags;
		pm::Pcb				*pcb = pm::k_pm.get_cur_pcb();
		mm::PageTable		*pt	 = pcb->get_pagetable();

		if ( _arg_addr( 0, bufaddr ) < 0 ) return -1;

		if ( _arg_int( 1, buflen ) < 0 ) return -1;

		if ( _arg_int( 2, buflen ) < 0 ) return -1;

		if ( bufaddr == 0 && buflen == 0 ) return -1;

		char *k_buf = new char[buflen];
		if ( !k_buf ) return -1;

		ulong  random	   = 0x4249'4C47'4B43'5546UL;
		size_t random_size = sizeof( random );
		for ( size_t i = 0; i < static_cast<size_t>( buflen ); i += random_size )
		{
			size_t copy_size =
				( i + random_size ) <= static_cast<size_t>( buflen ) ? random_size : buflen - i;
			memcpy( k_buf + i, &random, copy_size );
		}
		if ( mm::k_vmm.copyout( *pt, bufaddr, k_buf, buflen ) < 0 ) return -1;

		delete[] k_buf;
		return buflen;
	}

	uint64 SyscallHandler::_sys_sigaction()
	{
		pm::Pcb									   *proc = pm::k_pm.get_cur_pcb();
		[[maybe_unused]] mm::PageTable			   *pt	 = proc->get_pagetable();
		[[maybe_unused]] pm::ipc::signal::sigaction a_newact, a_oldact;
		// a_newact = nullptr;
		// a_oldact = nullptr;
		uint64										newactaddr, oldactaddr;
		int											flag;
		int											ret = -1;

		if ( _arg_int( 0, flag ) < 0 ) return -1;

		if ( _arg_addr( 1, newactaddr ) < 0 ) return -1;

		if ( _arg_addr( 2, oldactaddr ) < 0 ) return -1;

		if ( oldactaddr != 0 ) a_oldact = pm::ipc::signal::sigaction();

		if ( newactaddr != 0 )
		{
			if ( mm::k_vmm.copy_in( *pt, &a_newact, newactaddr,
									sizeof( pm::ipc::signal::sigaction ) ) < 0 )
				return -1;
			// a_newact = ( pm::ipc::signal::sigaction *)(hsai::k_mem->to_vir(
			// pt->walk_addr( newactaddr ) ));
			ret = pm::ipc::signal::sigAction( flag, &a_newact, nullptr );
		}
		else { ret = pm::ipc::signal::sigAction( flag, &a_newact, &a_oldact ); }
		if ( ret == 0 && oldactaddr != 0 )
		{
			if ( mm::k_vmm.copyout( *pt, oldactaddr, &a_oldact,
									sizeof( pm::ipc::signal::sigaction ) ) < 0 )
				return -1;
		}
		return ret;
	}

	uint64 SyscallHandler::_sys_ioctl()
	{
		int tmp;

		fs::file *f = nullptr;
		int		  fd;
		if ( _arg_fd( 0, &fd, &f ) < 0 ) return -1;
		if ( f == nullptr ) return -1;
		fd = fd;

		if ( f->_attrs.filetype != fs::FileTypes::FT_DEVICE ) return -1;

		u32 cmd;
		if ( _arg_int( 1, tmp ) < 0 ) return -2;
		cmd = (u32) tmp;
		cmd = cmd;

		ulong arg;
		if ( _arg_addr( 2, arg ) < 0 ) return -3;
		arg = arg;

		/// @todo not implement

		if ( ( cmd & 0xFFFF ) == TCGETS )
		{
			fs::device_file *df = (fs::device_file *) f;
			mm::PageTable	*pt = pm::k_pm.get_cur_pcb()->get_pagetable();
			termios			*ts = (termios *) hsai::k_mem->to_vir( pt->walk_addr( arg ) );
			return df->tcgetattr( ts );
		}

		if ( ( cmd & 0XFFFF ) == TIOCGPGRP )
		{
			mm::PageTable *pt	  = pm::k_pm.get_cur_pcb()->get_pagetable();
			int			  *p_pgrp = (int *) hsai::k_mem->to_vir( pt->walk_addr( arg ) );
			*p_pgrp				  = 1;
			return 0;
		}

		return 0;
	}

	uint64 SyscallHandler::_sys_fcntl()
	{
		pm::Pcb	 *p = pm::k_pm.get_cur_pcb();
		fs::file *f = nullptr;
		int		  op;
		ulong	  arg;
		int		  retfd = -1;

		if ( _arg_fd( 0, nullptr, &f ) < 0 ) return -1;
		if ( _arg_int( 1, op ) < 0 ) return -2;

		switch ( op )
		{
			case F_SETFD:
				if ( _arg_addr( 2, arg ) < 0 ) return -3;
				if ( arg & FD_CLOEXEC ) f->_fl_cloexec = true;
				return 0;

			case F_DUPFD:
				if ( _arg_addr( 2, arg ) < 0 ) return -3;
				for ( int i = (int) arg; i < (int) pm::max_open_files; ++i )
				{
					if ( ( retfd = pm::k_pm.alloc_fd( p, f, i ) ) == i )
					{
						f->refcnt++;
						break;
					}
				}
				return retfd;

			case F_DUPFD_CLOEXEC:
				if ( _arg_addr( 2, arg ) < 0 ) return -3;
				for ( int i = (int) arg; i < (int) pm::max_open_files; ++i )
				{
					if ( ( retfd = pm::k_pm.alloc_fd( p, f, i ) ) == i )
					{
						f->refcnt++;
						break;
					}
				}
				p->get_open_file( retfd )->_fl_cloexec = true;
				return retfd;

			default: break;
		}

		return retfd;
	}

	uint64 SyscallHandler::_sys_getpgid()
	{
		int pid;
		if ( _arg_int( 0, pid ) < 0 ) return -1;
		return 1;
	}

	uint64 SyscallHandler::_sys_setpgid()
	{
		int pid, pgid;
		if ( _arg_int( 0, pid ) < 0 || _arg_int( 1, pgid ) < 0 ) return -1;
		return 1;
	}

	uint64 SyscallHandler::_sys_geteuid() { return 1; }

	uint64 SyscallHandler::_sys_ppoll()
	{
		uint64					  fds_addr;
		uint64					  timeout_addr;
		uint64					  sigmask_addr;
		pollfd					 *fds = nullptr;
		int						  nfds;
		[[maybe_unused]] timespec tm{ 0, 0 }; // 现在没用上
		[[maybe_unused]] sigset_t sigmask;	  // 现在没用上
		[[maybe_unused]] int	  timeout;	  // 现在没用上
		int						  ret = 0;

		pm::Pcb		  *proc = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt	= proc->get_pagetable();

		if ( _arg_addr( 0, fds_addr ) < 0 ) return -1;

		if ( _arg_int( 1, nfds ) < 0 ) return -1;

		if ( _arg_addr( 2, timeout_addr ) < 0 ) return -1;

		if ( _arg_addr( 3, sigmask_addr ) < 0 ) return -1;

		fds = new pollfd[nfds];
		if ( fds == nullptr ) return -2;
		for ( int i = 0; i < nfds; i++ )
		{
			if ( mm::k_vmm.copy_in( *pt, &fds[i], fds_addr + i * sizeof( pollfd ),
									sizeof( pollfd ) ) < 0 )
			{
				delete[] fds;
				return -1;
			}
		}

		if ( timeout_addr != 0 )
		{
			if ( ( mm::k_vmm.copy_in( *pt, &tm, timeout_addr, sizeof( tm ) ) ) < 0 )
			{
				delete[] fds;
				return -1;
			}
			timeout = tm.tv_sec * 1000 + tm.tv_nsec / 1'000'000;
		}
		else
			timeout = -1;

		if ( sigmask_addr != 0 )
			if ( mm::k_vmm.copy_in( *pt, &sigmask, sigmask_addr, sizeof( sigset_t ) ) < 0 )
			{
				delete[] fds;
				return -1;
			}

		while ( 1 )
		{
			for ( auto i = 0; i < nfds; i++ )
			{
				fds[i].revents = 0;
				if ( fds[i].fd < 0 ) { continue; }

				fs::file *f	   = nullptr;
				int		  reti = 0;

				if ( ( f = proc->get_open_file( fds[i].fd ) ) == nullptr )
				{
					fds[i].revents |= POLLNVAL;
					reti			= 1;
				}
				else
				{
					if ( fds[i].events & POLLIN )
					{
						if ( f->read_ready() )
						{
							fds[i].revents |= POLLIN;
							reti			= 1;
						}
					}
					if ( fds[i].events & POLLOUT )
					{
						if ( f->write_ready() )
						{
							fds[i].revents |= POLLOUT;
							reti			= 1;
						}
					}
				}

				ret += reti;
			}
			if ( ret != 0 ) break;
			// else
			// {
			// 	/// @todo sleep
			// }
		}

		if ( mm::k_vmm.copyout( *pt, fds_addr, fds, nfds * sizeof( pollfd ) ) < 0 )
		{
			delete[] fds;
			return -1;
		}

		delete[] fds;
		return ret;
	}

	uint64 SyscallHandler::_sys_getgid() { return 1; }

	uint64 SyscallHandler::_sys_setgid() { return 1; }

	uint64 SyscallHandler::_sys_setuid() { return 1; }

	uint64 SyscallHandler::_sys_sendfile()
	{
		int		  in_fd, out_fd;
		fs::file *in_f, *out_f;
		if ( _arg_fd( 0, &out_fd, &out_f ) < 0 ) return -1;
		if ( _arg_fd( 1, &in_fd, &in_f ) < 0 ) return -2;

		ulong  addr;
		ulong *p_off = nullptr;
		p_off		 = p_off;
		if ( _arg_addr( 2, addr ) < 0 ) return -3;

		mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
		if ( addr != 0 ) p_off = (ulong *) hsai::k_mem->to_vir( pt->walk_addr( addr ) );

		size_t count;
		if ( _arg_addr( 3, count ) < 0 ) return -4;

		/// @todo sendfile

		ulong start_off = in_f->get_file_offset();
		if ( p_off != nullptr ) start_off = *p_off;

		char *buf = new char[count + 1];
		if ( buf == nullptr ) return -5;

		int readcnt	 = in_f->read( (ulong) buf, count, start_off, true );
		int writecnt = 0;
		if ( out_f->_attrs.filetype == fs::FileTypes::FT_PIPE )
			writecnt = ( (fs::pipe_file *) out_f )->write_in_kernel( (ulong) buf, readcnt );
		else
			writecnt = out_f->write( (ulong) buf, readcnt, out_f->get_file_offset(), true );

		delete[] buf;

		if ( p_off != nullptr ) *p_off += writecnt;

		return writecnt;
	}

	uint64 SyscallHandler::_sys_exit_group()
	{
		int status;
		if ( _arg_int( 0, status ) < 0 ) return -1;
		pm::k_pm.exit_group( status );
		return -111; // not return;
	}

	uint64 SyscallHandler::_sys_statfs()
	{
		eastl::string  path;
		uint64		   statfsaddr;
		mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();

		if ( _arg_str( 0, path, 128 ) < 0 ) return -1;

		if ( _arg_addr( 1, statfsaddr ) < 0 ) return -2;

		fs::Path   fspath( path );
		fs::statfs statfs_ = fs::statfs( *fspath.pathSearch()->getNode()->getFS() );

		if ( mm::k_vmm.copyout( *pt, statfsaddr, &statfs_, sizeof( statfs_ ) ) < 0 ) return -1;
		return 0;
	}

	uint64 SyscallHandler::_sys_syslog()
	{
		enum sys_log_type
		{

			SYSLOG_ACTION_CLOSE			= 0,
			SYSLOG_ACTION_OPEN			= 1,
			SYSLOG_ACTION_READ			= 2,
			SYSLOG_ACTION_READ_ALL		= 3,
			SYSLOG_ACTION_READ_CLEAR	= 4,
			SYSLOG_ACTION_CLEAR			= 5,
			SYSLOG_ACTION_CONSOLE_OFF	= 6,
			SYSLOG_ACTION_CONSOLE_ON	= 7,
			SYSLOG_ACTION_CONSOLE_LEVEL = 8,
			SYSLOG_ACTION_SIZE_UNREAD	= 9,
			SYSLOG_ACTION_SIZE_BUFFER	= 10

		};

		int			  prio;
		eastl::string fmt;
		uint64		  fmt_addr;
		eastl::string msg = "Spectre V2 : Update user space SMT mitigation: STIBP always-on\n"
							"process_manager : execve set stack-base = 0x0000_0000_9194_5000\n"
							"pm/process_manager : execve set page containing sp is 0x0000_0000_9196_4000";
		[[maybe_unused]] pm::Pcb	   *p  = pm::k_pm.get_cur_pcb();
		[[maybe_unused]] mm::PageTable *pt = p->get_pagetable();

		if ( _arg_int( 0, prio ) < 0 ) return -1;

		if ( _arg_addr( 1, fmt_addr ) < 0 ) return -1;


		if ( prio == SYSLOG_ACTION_SIZE_BUFFER )
			return msg.size(); // 返回buffer的长度
		else if ( prio == SYSLOG_ACTION_READ_ALL )
		{
			mm::k_vmm.copyout( *pt, fmt_addr, msg.c_str(), msg.size() );
			return msg.size();
		}

		return 0;
	}

	uint64 SyscallHandler::_sys_faccessat()
	{
		int			  _dirfd;
		uint64		  _pathaddr;
		eastl::string _pathname;
		int			  _mode;
		int			  _flags;

		if ( _arg_int( 0, _dirfd ) < 0 ) return -1;

		if ( _arg_addr( 1, _pathaddr ) < 0 ) return -1;

		if ( _arg_int( 2, _mode ) < 0 ) return -1;

		if ( _arg_int( 3, _flags ) < 0 ) return -1;
		pm::Pcb		  *cur_proc = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt		= cur_proc->get_pagetable();

		if ( mm::k_vmm.copy_str_in( *pt, _pathname, _pathaddr, 100 ) < 0 ) return -1;
		if ( _pathname.empty() ) return -1;

		[[maybe_unused]] int flags = 0;
		// if( ( _mode & ( R_OK | X_OK )) && ( _mode & W_OK ) )
		// 	flags = 6;    	//O_RDWR;
		// else if( _mode & W_OK )
		// 	flags = 2;		//O_WRONLY + 1;
		// else if( _mode & ( R_OK | X_OK ))
		// 	flags = 4		//O_RDONLY + 1;

		if ( _mode & R_OK ) flags |= 4;
		if ( _mode & W_OK ) flags |= 2;
		if ( _mode & X_OK ) flags |= 1;

		fs::Path path;
		if ( _dirfd == -100 ) // AT_CWD
			new ( &path ) fs::Path( _pathname, cur_proc->_cwd );
		else
			new ( &path ) fs::Path( _pathname, cur_proc->_ofile[_dirfd] );

		int fd = path.open( fs::FileAttrs( flags ) );

		if ( fd < 0 ) return -1;


		return 0;
	}

	uint64 SyscallHandler::_sys_sysinfo()
	{
		// struct sysinfo {
		// 	__kernel_long_t uptime;		/* Seconds since boot */
		// 	__kernel_ulong_t loads[3];	/* 1, 5, and 15 minute load averages */
		// 	__kernel_ulong_t totalram;	/* Total usable main memory size */
		// 	__kernel_ulong_t freeram;	/* Available memory size */
		// 	__kernel_ulong_t sharedram;	/* Amount of shared memory */
		// 	__kernel_ulong_t bufferram;	/* Memory used by buffers */
		// 	__kernel_ulong_t totalswap;	/* Total swap space size */
		// 	__kernel_ulong_t freeswap;	/* swap space still available */
		// 	__u16 procs;		   	/* Number of current processes */
		// 	__u16 pad;		   	/* Explicit padding for m68k */
		// 	__kernel_ulong_t totalhigh;	/* Total high memory size */
		// 	__kernel_ulong_t freehigh;	/* Available high memory size */
		// 	__u32 mem_unit;			/* Memory unit size in bytes */
		// 	char _f[20-2*sizeof(__kernel_ulong_t)-sizeof(__u32)];	/* Padding:
		// libc5 uses this.. */
		// };


		uint64					 sysinfoaddr;
		[[maybe_unused]] sysinfo sysinfo_;

		if ( _arg_addr( 0, sysinfoaddr ) < 0 ) return -1;

		pm::Pcb		  *cur_proc = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt		= cur_proc->get_pagetable();

		memset( &sysinfo_, 0, sizeof( sysinfo_ ) );
		sysinfo_.uptime	   = 0;
		sysinfo_.loads[0]  = 0; // 负载均值  1min 5min 15min
		sysinfo_.loads[1]  = 0;
		sysinfo_.loads[2]  = 0;
		sysinfo_.totalram  = 0; // 总内存
		sysinfo_.freeram   = 0;
		sysinfo_.sharedram = 0;
		sysinfo_.bufferram = 0;
		sysinfo_.totalswap = 0;
		sysinfo_.freeswap  = 0;
		sysinfo_.procs	   = 0;
		sysinfo_.pad	   = 0;
		sysinfo_.totalhigh = 0;
		sysinfo_.freehigh  = 0;
		sysinfo_.mem_unit  = 1; // 内存单位为 1 字节

		if ( mm::k_vmm.copyout( *pt, sysinfoaddr, &sysinfo_, sizeof( sysinfo_ ) ) < 0 ) return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_nanosleep()
	{
		int		 clockid;
		int		 flags;
		timespec dur;
		uint64	 dur_addr;
		timespec rem;
		uint64	 rem_addr;

		if ( _arg_int( 0, clockid ) < 0 ) return -1;

		if ( _arg_int( 1, flags ) < 0 ) return -1;

		if ( _arg_addr( 2, dur_addr ) < 0 ) return -1;

		if ( _arg_addr( 3, rem_addr ) < 0 ) return -2;

		pm::Pcb		  *cur_proc = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt		= cur_proc->get_pagetable();

		if ( dur_addr != 0 )
			if ( mm::k_vmm.copy_in( *pt, &dur, dur_addr, sizeof( dur ) ) < 0 ) return -1;

		if ( rem_addr != 0 )
			if ( mm::k_vmm.copy_in( *pt, &rem, rem_addr, sizeof( rem ) ) < 0 ) return -1;

		tmm::timeval tm_;
		tm_.tv_sec	= dur.tv_sec;
		tm_.tv_usec = dur.tv_nsec / 1000;

		tmm::k_tm.sleep_from_tv( tm_ );

		return 0;
	}

	uint64 SyscallHandler::_sys_getrusage()
	{
		struct rusage
		{
			/* Total amount of user time used.  */
			struct timeval ru_utime;
			/* Total amount of system time used.  */
			struct timeval ru_stime;
			/* Maximum resident set size (in kilobytes).  */
			__extension__ union
			{
				long int		  ru_maxrss;
				__syscall_slong_t __ru_maxrss_word;
			};
			/* Amount of sharing of text segment memory
			with other processes (kilobyte-seconds).  */
			__extension__ union
			{
				long int		  ru_ixrss;
				__syscall_slong_t __ru_ixrss_word;
			};
			/* Amount of data segment memory used (kilobyte-seconds).  */
			__extension__ union
			{
				long int		  ru_idrss;
				__syscall_slong_t __ru_idrss_word;
			};
			/* Amount of stack memory used (kilobyte-seconds).  */
			__extension__ union
			{
				long int		  ru_isrss;
				__syscall_slong_t __ru_isrss_word;
			};
			/* Number of soft page faults (i.e. those serviced by reclaiming
			a page from the list of pages awaiting reallocation.  */
			__extension__ union
			{
				long int		  ru_minflt;
				__syscall_slong_t __ru_minflt_word;
			};
			/* Number of hard page faults (i.e. those that required I/O).  */
			__extension__ union
			{
				long int		  ru_majflt;
				__syscall_slong_t __ru_majflt_word;
			};
			/* Number of times a process was swapped out of physical memory.  */
			__extension__ union
			{
				long int		  ru_nswap;
				__syscall_slong_t __ru_nswap_word;
			};
			/* Number of input operations via the file system.  Note: This
			and `ru_oublock' do not include operations with the cache.  */
			__extension__ union
			{
				long int		  ru_inblock;
				__syscall_slong_t __ru_inblock_word;
			};
			/* Number of output operations via the file system.  */
			__extension__ union
			{
				long int		  ru_oublock;
				__syscall_slong_t __ru_oublock_word;
			};
			/* Number of IPC messages sent.  */
			__extension__ union
			{
				long int		  ru_msgsnd;
				__syscall_slong_t __ru_msgsnd_word;
			};
			/* Number of IPC messages received.  */
			__extension__ union
			{
				long int		  ru_msgrcv;
				__syscall_slong_t __ru_msgrcv_word;
			};
			/* Number of signals delivered.  */
			__extension__ union
			{
				long int		  ru_nsignals;
				__syscall_slong_t __ru_nsignals_word;
			};
			/* Number of voluntary context switches, i.e. because the process
			gave up the process before it had to (usually to wait for some
			resource to be available).  */
			__extension__ union
			{
				long int		  ru_nvcsw;
				__syscall_slong_t __ru_nvcsw_word;
			};
			/* Number of involuntary context switches, i.e. a higher priority process
			became runnable or the current process used up its time slice.  */
			__extension__ union
			{
				long int		  ru_nivcsw;
				__syscall_slong_t __ru_nivcsw_word;
			};
		};

		enum __rusage_who
		{
			/* The calling process.  */
			RUSAGE_SELF = 0,
#define RUSAGE_SELF RUSAGE_SELF

			/* All of its terminated child processes.  */
			RUSAGE_CHILDREN = -1
#define RUSAGE_CHILDREN RUSAGE_CHILDREN

#ifdef __USE_GNU
			,
			/* The calling thread.  */
			RUSAGE_THREAD = 1
#	define RUSAGE_THREAD RUSAGE_THREAD
/* Name for the same functionality on Solaris.  */
#	define RUSAGE_LWP	  RUSAGE_THREAD
#endif
		};

		int	   who;
		uint64 rusage_addr;

		if ( _arg_int( 0, who ) < 0 ) return -1;

		if ( _arg_addr( 1, rusage_addr ) < 0 ) return -1;


		rusage		   rusage_;
		pm::Pcb		  *cur_proc = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt		= cur_proc->get_pagetable();
		tmm::tms	   tms;
		pm::k_pm.get_cur_proc_tms( &tms );

		switch ( who )
		{
			case RUSAGE_SELF:
				rusage_.ru_utime.tv_sec	 = tms.tms_utime;
				rusage_.ru_utime.tv_usec = tms.tms_cutime;
				rusage_.ru_maxrss		 = cur_proc->get_max_rss();
				break;
			case RUSAGE_CHILDREN:
				// 添加对 RUSAGE_CHILDREN 的处理逻辑
				// 假设有一个函数 get_children_rusage() 来获取子进程的资源使用情况
				// rusage_ = cur_proc->get_children_rusage();
				break;
			default: log_error( "get_rusage: invalid who\n" ); return -1;
		}
		rusage_.ru_ixrss	= 0;
		rusage_.ru_idrss	= 0;
		rusage_.ru_isrss	= 0;
		rusage_.ru_minflt	= 0; /// @todo: 缺页中断的次数
		rusage_.ru_majflt	= 0; /// @todo: 页错误次数
		rusage_.ru_nswap	= 0;
		rusage_.ru_inblock	= 0;
		rusage_.ru_oublock	= 0;
		rusage_.ru_msgsnd	= 0;
		rusage_.ru_msgrcv	= 0;
		rusage_.ru_nsignals = 0;
		rusage_.ru_nvcsw	= 0;
		rusage_.ru_nivcsw	= 0;

		if ( mm::k_vmm.copyout( *pt, rusage_addr, &rusage_, sizeof( rusage_ ) ) < 0 ) return -1;

		return 0;
	}

} // namespace syscall

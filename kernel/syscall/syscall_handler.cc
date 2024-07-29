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
#include "fs/file/file.hh"
#include "fs/dev/acpi_controller.hh"
#include "fs/kstat.hh"
#include "fs/path.hh"
#include "fs/file/normal.hh"
#include "fs/file/file.hh"

#include "pm/process.hh"
// #include "pm/trap_frame.hh"
#include "pm/process_manager.hh"
#include "pm/scheduler.hh"
#include "mm/virtual_memory_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "tm/timer_manager.hh"
#include "klib/klib.hh"

#include <hsai_global.hh>
#include <mem/virtual_memory.hh>
#include <process_interface.hh>


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
		_syscall_funcs[ SYS_read ] = std::bind( &SyscallHandler::_sys_read, this );
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
		_syscall_funcs[ SYS_sched_yield ] = std::bind( &SyscallHandler::_sys_sched_yield, this );
		_syscall_funcs[ SYS_sleep ] = std::bind( &SyscallHandler::_sys_sleep, this );
		_syscall_funcs[ SYS_times ] = std::bind( &SyscallHandler::_sys_times, this );
		_syscall_funcs[ SYS_uname ] = std::bind( &SyscallHandler::_sys_uname, this );
		_syscall_funcs[ SYS_openat ] = std::bind( &SyscallHandler::_sys_openat, this );
		_syscall_funcs[ SYS_close ] = std::bind( &SyscallHandler::_sys_close, this );
		_syscall_funcs[ SYS_fstat ] = std::bind( &SyscallHandler::_sys_fstat, this );
		_syscall_funcs[ SYS_getdents ] = std::bind( &SyscallHandler::_sys_getdents, this );
		_syscall_funcs[ SYS_mkdir ] = std::bind( &SyscallHandler::_sys_mkdir, this );
		_syscall_funcs[ SYS_chdir ] = std::bind( &SyscallHandler::_sys_chdir, this );
		_syscall_funcs[ SYS_mount ] = std::bind( &SyscallHandler::_sys_mount, this );
		_syscall_funcs[ SYS_umount ] = std::bind( &SyscallHandler::_sys_umount, this );
		_syscall_funcs[ SYS_mmap ] = std::bind( &SyscallHandler::_sys_mmap, this );
		_syscall_funcs[ SYS_munmap ] = std::bind( &SyscallHandler::_sys_munmap, this );
		_syscall_funcs[ SYS_statx ] = std::bind( &SyscallHandler::_sys_statx, this );
		_syscall_funcs[ SYS_unlinkat ] = std::bind( &SyscallHandler::_sys_unlinkat, this );
		_syscall_funcs[ SYS_pipe ] = std::bind( &SyscallHandler::_sys_pipe, this );
		_syscall_funcs[ SYS_set_tid_address ] = std::bind( &SyscallHandler::_sys_set_tid_address, this );
	}

	uint64 SyscallHandler::invoke_syscaller( uint64 sys_num )
	{
		printf( BLUE_COLOR_PRINT "invoke syscall %d\n" CLEAR_COLOR_PRINT, sys_num );
		return _syscall_funcs[ sys_num ]();
	}


// ---------------- private helper functions ----------------

	int SyscallHandler::_fetch_addr( uint64 addr, uint64 &out_data )
	{
		pm::Pcb *p = ( pm::Pcb * ) hsai::get_cur_proc();
		if ( addr >= p->get_size() || addr + sizeof( uint64 ) > p->get_size() )
			return -1;
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_in( *pt, &out_data, addr, sizeof( out_data ) ) < 0 )
			return -1;
		return 0;
	}

	int SyscallHandler::_fetch_str( uint64 addr, void *buf, uint64 max )
	{
		pm::Pcb *p = ( pm::Pcb * ) hsai::get_cur_proc();
		mm::PageTable *pt = p->get_pagetable();
		int err = mm::k_vmm.copy_str_in( *pt, buf, addr, max );
		if ( err < 0 )
			return err;
		return strlen( ( const char * ) buf );
	}

	int SyscallHandler::_fetch_str( uint64 addr, eastl::string &str, uint64 max )
	{
		pm::Pcb *p = ( pm::Pcb * ) hsai::get_cur_proc();
		mm::PageTable *pt = p->get_pagetable();
		int err = mm::k_vmm.copy_str_in( *pt, str, addr, max );
		if ( err < 0 )
			return err;
		return str.size();
	}

	uint64 SyscallHandler::_arg_raw( int arg_n )
	{
		return hsai::get_arg_from_trap_frame( hsai::get_trap_frame_from_proc( hsai::get_cur_proc() ), ( uint ) arg_n );
	}

	int SyscallHandler::_arg_fd( int arg_n, int *out_fd, fs::file **out_f )
	{
		int fd;
		fs::file *f;

		if ( _arg_int( arg_n, fd ) < 0 )
			return -1;
		pm::Pcb *p = ( pm::Pcb * ) hsai::get_cur_proc();
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
		fs::file *f;
		int n;
		uint64 p;

		if ( _arg_fd( 0, nullptr, &f ) < 0
			|| _arg_addr( 1, p ) < 0
			|| _arg_int( 2, n ) < 0 )
		{
			return -1;
		}

		pm::Pcb * proc = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = proc->get_pagetable();

		u64 kp = pt->walk_addr( p );
		kp = hsai::k_mem->to_vir( kp );

		return f->write( kp, n );
	}

	uint64 SyscallHandler::_sys_read()
	{
		fs::file * f;
		uint64 buf;
		int n;

		if ( _arg_fd( 0, nullptr, &f ) < 0 )
			return -1;
		if ( _arg_addr( 1, buf ) < 0 )
			return -2;
		if ( _arg_int( 2, n ) < 0 )
			return -3;

		if ( f == nullptr )
			return -4;
		if ( n <= 0 )
			return -5;

		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();

		char * k_buf = new char[ n + 1 ];
		int ret = f->read( ( uint64 ) k_buf, n );
		if ( ret < 0 )
			return -6;
		if ( mm::k_vmm.copyout( *pt, buf, k_buf, ret ) < 0 )
			return -7;

		delete[] k_buf;
		return ret;
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
		uint64 u_sp;
		if ( _arg_addr( 1, u_sp ) < 0 )
			return -1;

		return pm::k_pm.fork( u_sp );
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
		ulong n;
		if ( _arg_addr( 0, n ) < 0 )
			return -1;
		return pm::k_pm.brk( n );
	}

	uint64 SyscallHandler::_sys_exec()
	{
		uint i;
		uint64 uargv, uarg;

		_path.clear();
		if ( _arg_str( 0, _path, hsai::page_size ) < 0 || _arg_addr( 1, uargv ) < 0 )
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
			if ( _fetch_str( uarg, _argv[ i ], hsai::page_size ) < 0 )
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
			// _argv.clear();
			ret = pm::k_pm.exec( _path, _argv );
		}

		_argv.clear();
		return ret;
	}

	uint64 SyscallHandler::_sys_wait()
	{
		int pid;
		uint64 wstatus_addr;
		if ( _arg_int( 0, pid ) < 0 )
			return -1;
		if ( _arg_addr( 1, wstatus_addr ) < 0 )
			return -1;
		return pm::k_pm.wait( pid, wstatus_addr );
	}

	uint64 SyscallHandler::_sys_poweroff()
	{
		dev::acpi::k_acpi_controller.power_off();
		return 0;
	}

	uint64 SyscallHandler::_sys_dup()
	{
		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		fs::file * f;
		int fd;

		if ( _arg_fd( 0, nullptr, &f ) < 0 )
			return -1;
		if ( ( fd = pm::k_pm.alloc_fd( p, f ) ) < 0 )
			return -1;
		//fs::k_file_table.dup( f );
		f->dup();
		return fd;
	}

	uint64 SyscallHandler::_sys_dup2()
	{
		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		fs::file * f;
		int fd;

		if ( _arg_fd( 0, nullptr, &f ) < 0 )
			return -1;
		if ( _arg_int( 1, fd ) < 0 )
			return -1;
		if ( pm::k_pm.alloc_fd( p, f, fd ) < 0 )
			return -1;
		//fs::k_file_table.dup( f );
		f->dup();
		return fd;
	}

	uint64 SyscallHandler::_sys_getcwd()
	{
		char cwd[ 256 ];
		uint64 buf;
		int size;

		if ( _arg_addr( 0, buf ) < 0 )
			return -1;
		if ( _arg_int( 1, size ) < 0 )
			return -1;
		if ( size >= ( int ) sizeof( cwd ) )
			return -1;

		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		uint len = pm::k_pm.getcwd( cwd );
		if ( mm::k_vmm.copyout( *pt, buf, ( const void * ) cwd, len ) < 0 )
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
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copyout( *pt, tv_addr, ( const void * ) &tv, sizeof( tv ) ) < 0 )
			return -1;

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
		uint64 tv_addr;

		if ( _arg_addr( 0, tv_addr ) < 0 )
			return -1;

		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_in( *pt, &tv, tv_addr, sizeof( tv ) ) < 0 )
			return -1;

		return tmm::k_tm.sleep_from_tv( tv );
	}

	uint64 SyscallHandler::_sys_times()
	{
		tmm::tms tms_val;
		uint64 tms_addr;

		if ( _arg_addr( 0, tms_addr ) < 0 )
			return -1;

		pm::k_pm.get_cur_proc_tms( &tms_val );

		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copyout( *pt, tms_addr, &tms_val, sizeof( tms_val ) ) < 0 )
			return -1;

		return tmm::k_tm.get_ticks();
	}

	struct _Utsname
	{
		char sysname[ 65 ];
		char nodename[ 65 ];
		char release[ 65 ];
		char version[ 65 ];
		char machine[ 65 ];
		char domainname[ 65 ];
	};
	static const char _SYSINFO_sysname[] = "XN6";
	static const char _SYSINFO_nodename[] = "(none-node)";
	static const char _SYSINFO_release[] = "V1.0";
	static const char _SYSINFO_version[] = "V1.0";
	static const char _SYSINFO_machine[] = "LoongArch-2k1000";
	static const char _SYSINFO_domainname[] = "(none-domain)";

	uint64 SyscallHandler::_sys_uname()
	{
		uint64 usta;
		uint64 sysa, noda, rlsa, vsna, mcha, dmna;

		if ( _arg_addr( 0, usta ) < 0 ) return -11;
		sysa = ( uint64 ) ( ( ( _Utsname* ) usta )->sysname );
		noda = ( uint64 ) ( ( ( _Utsname* ) usta )->nodename );
		rlsa = ( uint64 ) ( ( ( _Utsname* ) usta )->release );
		vsna = ( uint64 ) ( ( ( _Utsname* ) usta )->version );
		mcha = ( uint64 ) ( ( ( _Utsname* ) usta )->machine );
		dmna = ( uint64 ) ( ( ( _Utsname* ) usta )->domainname );


		pm::Pcb * p = pm::k_pm.get_cur_pcb();
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
		if ( mm::k_vmm.copyout( *pt, dmna, _SYSINFO_domainname, sizeof( _SYSINFO_domainname ) ) < 0 )
			return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_openat()
	{
		int dir_fd;
		uint64 path_addr;
		int flags;

		if ( _arg_int( 0, dir_fd ) < 0 )
			return -1;
		if ( _arg_addr( 1, path_addr ) < 0 )
			return -1;
		if ( _arg_int( 2, flags ) < 0 )
			return -1;

		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		eastl::string path;
		if ( mm::k_vmm.copy_str_in( *pt, path, path_addr, 100 ) < 0 )
			return -1;

		int res = pm::k_pm.open( dir_fd, path, flags );
		return res;
	}

	uint64 SyscallHandler::_sys_close()
	{
		int fd;

		if ( _arg_int( 0, fd ) < 0 )
			return -1;
		return pm::k_pm.close( fd );
	}

	uint64 SyscallHandler::_sys_fstat()
	{
		int fd;
		fs::Kstat kst;
		uint64 kst_addr;

		if ( _arg_int( 0, fd ) < 0 )
			return -1;

		if ( _arg_addr( 1, kst_addr ) < 0 )
			return -1;

		pm::k_pm.fstat( fd, &kst );
		mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
		if ( mm::k_vmm.copyout( *pt, kst_addr, &kst, sizeof( kst ) ) < 0 )
			return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_getdents()
	{
		// 这个定义来自 Linux
		struct linux_dirent
		{
			unsigned long  d_ino;     /* Inode number */
			unsigned long  d_off;     /* Offset to next linux_dirent */
			unsigned short d_reclen;  /* Length of this linux_dirent */
			unsigned char  d_type;
			char           d_name[ 128 - sizeof( d_ino ) - sizeof( d_off ) - sizeof( d_reclen ) - sizeof( d_type ) ];  /* Filename (null-terminated) */
							  /* length is actually (d_reclen - 2 -
								 offsetof(struct linux_dirent, d_name)) */
			/*
			char           pad;       // Zero padding byte
			char           d_type;    // File type (only since Linux
									  // 2.6.4); offset is (d_reclen - 1)
			*/
		} dirent;
		dirent.d_reclen = 128;

		fs::file * f;
		uint64 buf_addr;
		int buf_len;

		if ( _arg_fd( 0, nullptr, &f ) < 0 )
			return -1;
		if ( _arg_addr( 1, buf_addr ) < 0 )
			return -1;
		if ( _arg_int( 2, buf_len ) < 0 )
			return -1;
		if( f->_attrs.filetype != fs::FileTypes::FT_NORMAL)
			return -1;
		//eastl::string name = f->data.get_Entry()->rName();
		fs::normal_file * normal_f= static_cast<fs::normal_file *>( f );
		eastl::string name = normal_f->getDentry()->rName();

		for ( uint i = 0; i < name.size(); ++i )
			dirent.d_name[ i ] = name[ i ];

		mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
		if ( mm::k_vmm.copyout( *pt, buf_addr, &dirent, sizeof( dirent ) ) < 0 )
			return -1;

		return sizeof( dirent );
	}

	uint64 SyscallHandler::_sys_mkdir()
	{
		return 0;
	}

	uint64 SyscallHandler::_sys_chdir()
	{
		eastl::string path;

		if ( _arg_str( 0, path, hsai::page_size ) < 0 )
			return -1;

		return pm::k_pm.chdir( path );
	}

	uint64 SyscallHandler::_sys_mount()
	{
		uint64 dev_addr;
		uint64 mnt_addr;
		uint64 fstype_addr;
		eastl::string dev;
		eastl::string mnt;
		eastl::string fstype;
		int flags;
		uint64 data;
		pm::Pcb *p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();

		if ( _arg_addr( 0, dev_addr ) < 0 )
			return -1;
		if ( _arg_addr( 1, mnt_addr ) < 0 )
			return -1;
		if ( _arg_addr( 2, fstype_addr ) < 0 )
			return -1;

		if ( mm::k_vmm.copy_str_in( *pt, dev, dev_addr, 100 ) < 0 )
			return -1;
		if ( mm::k_vmm.copy_str_in( *pt, mnt, mnt_addr, 100 ) < 0 )
			return -1;
		if ( mm::k_vmm.copy_str_in( *pt, fstype, fstype_addr, 100 ) < 0 )
			return -1;

		if ( _arg_int( 3, flags ) < 0 )
			return -1;
		if ( _arg_addr( 4, data ) < 0 )
			return -1;

		//return pm::k_pm.mount( dev, mnt, fstype, flags, data );
		fs::Path devpath( dev );
		fs::Path mntpath( mnt );

		return devpath.mount( mntpath, fstype, flags, data );
	}

	uint64 SyscallHandler::_sys_umount()
	{
		uint64 specialaddr;
		eastl::string special;
		int flags;

		pm::Pcb * cur = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = cur->get_pagetable();

		if ( _arg_addr( 0, specialaddr ) < 0 )
			return -1;
		if ( _arg_int( 1, flags ) < 0 )
			return -1;

		if ( mm::k_vmm.copy_str_in( *pt, special, specialaddr, 100 ) < 0 )
			return -1;

		fs::Path specialpath( special );
		return specialpath.umount( flags );
	}

	uint64 SyscallHandler::_sys_mmap()
	{
		int map_size;
		int fd;

		if ( _arg_int( 1, map_size ) < 0 )
			return -1;

		if ( _arg_int( 4, fd ) < 0 )
			return -1;

		return pm::k_pm.mmap( fd, map_size );
	}

	uint64 SyscallHandler::_sys_munmap()
	{
		return 0;
	}

	uint64 SyscallHandler::_sys_statx()
	{
		using __u16 = uint16;
		using __u32 = uint32;
		using __s64 = int64;
		using __u64 = uint64;
		struct statx_timestamp
		{
			__s64 tv_sec;    /* Seconds since the Epoch (UNIX time) */
			__u32 tv_nsec;   /* Nanoseconds since tv_sec */
		};
		struct statx
		{
			__u32 stx_mask;        /* Mask of bits indicating
									  filled fields */
			__u32 stx_blksize;     /* Block size for filesystem I/O */
			__u64 stx_attributes;  /* Extra file attribute indicators */
			__u32 stx_nlink;       /* Number of hard links */
			__u32 stx_uid;         /* User ID of owner */
			__u32 stx_gid;         /* Group ID of owner */
			__u16 stx_mode;        /* File type and mode */
			__u64 stx_ino;         /* Inode number */
			__u64 stx_size;        /* Total size in bytes */
			__u64 stx_blocks;      /* Number of 512B blocks allocated */
			__u64 stx_attributes_mask;
								   /* Mask to show what's supported
									  in stx_attributes */

			/* The following fields are file timestamps */
			struct statx_timestamp stx_atime;  /* Last access */
			struct statx_timestamp stx_btime;  /* Creation */
			struct statx_timestamp stx_ctime;  /* Last status change */
			struct statx_timestamp stx_mtime;  /* Last modification */

			/* If this file represents a device, then the next two
			   fields contain the ID of the device */
			__u32 stx_rdev_major;  /* Major ID */
			__u32 stx_rdev_minor;  /* Minor ID */

			/* The next two fields contain the ID of the device
			   containing the filesystem where the file resides */
			__u32 stx_dev_major;   /* Major ID */
			__u32 stx_dev_minor;   /* Minor ID */

			__u64 stx_mnt_id;      /* Mount ID */

			/* Direct I/O alignment restrictions */
			__u32 stx_dio_mem_align;
			__u32 stx_dio_offset_align;
		};

		int fd;
		eastl::string path_name;
		fs::Kstat kst;
		statx stx;
		uint64 kst_addr;

		if ( _arg_int( 0, fd ) < 0 )
			return -1;

		if ( _arg_str( 1, path_name, 128 ) < 0 )
			return -1;

		if ( _arg_addr( 4, kst_addr ) < 0 )
			return -1;

		if ( fd > 0 )
		{
			pm::k_pm.fstat( fd, &kst );
			mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
			if ( mm::k_vmm.copyout( *pt, kst_addr, &kst, sizeof( kst ) ) < 0 )
				return -1;
			return 0;
		}
		else
		{
			int ffd;
			ffd = pm::k_pm.open( fd, path_name, 2 );
			if ( ffd < 0 )
				return -1;
			pm::k_pm.fstat( ffd, &kst );
			pm::k_pm.close( ffd );
			stx.stx_size = kst.size;
			mm::PageTable *pt = pm::k_pm.get_cur_pcb()->get_pagetable();
			if ( mm::k_vmm.copyout( *pt, kst_addr, &stx, sizeof( stx ) ) < 0 )
				return -1;
			return 0;
		}
	}

	uint64 SyscallHandler::_sys_unlinkat()
	{
		int fd, flags;
		uint64 path_addr;

		if ( _arg_int( 0, fd ) < 0 )
			return -1;
		if ( _arg_addr( 1, path_addr ) < 0 )
			return -1;
		if ( _arg_int( 2, flags ) < 0 )
			return -1;
		eastl::string path;
		pm::Pcb * p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_str_in( *pt, path, path_addr, 100 ) < 0 )
			return -1;

		int res = pm::k_pm.unlink( fd, path, flags );
		return res;
	}

	uint64 SyscallHandler::_sys_pipe()
	{
		int fd[ 2 ];
		uint64 addr;

		if ( _arg_addr( 0, addr ) < 0 )
			return -1;

		pm::Pcb *p = pm::k_pm.get_cur_pcb();
		mm::PageTable *pt = p->get_pagetable();
		if ( mm::k_vmm.copy_in( *pt, &fd, addr, 2 * sizeof( fd[ 0 ] ) ) < 0 )
			return -1;

		if ( pm::k_pm.pipe( fd, 0 ) < 0 )
			return -1;

		if ( mm::k_vmm.copyout( *pt, addr, &fd, 2 * sizeof( fd[ 0 ] ) ) < 0 )
			return -1;

		return 0;
	}

	uint64 SyscallHandler::_sys_set_tid_address()
	{
		ulong addr;
		int * tidptr;

		if ( _arg_addr( 0, addr ) < 0 )
			return -1;

		tidptr = ( int * ) addr;

		return pm::k_pm.set_tid_address( tidptr );
	}

} // namespace syscall

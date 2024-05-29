//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace syscall
{

#define _build_sysnum_enum_( name, number ) SYS_##name = number, 
	enum SyscallNumber
	{
		_build_sysnum_enum_( getcwd, 17 )
		_build_sysnum_enum_( dup, 23 )
		_build_sysnum_enum_( dup2, 24 )
		_build_sysnum_enum_( mkdir, 34 )
		_build_sysnum_enum_( unlinkat, 35 )
		_build_sysnum_enum_( umount, 39 )
		_build_sysnum_enum_( mount, 40 )
		_build_sysnum_enum_( chdir, 49 )
		_build_sysnum_enum_( openat, 56 )
		_build_sysnum_enum_( close, 57 )
		_build_sysnum_enum_( getdents, 61 )
		_build_sysnum_enum_( read, 63 )
		_build_sysnum_enum_( write, 64 )
		_build_sysnum_enum_( fstat, 80 )
		_build_sysnum_enum_( exit, 93 )
		_build_sysnum_enum_( sleep, 101 )
		_build_sysnum_enum_( sched_yield, 124 )
		_build_sysnum_enum_( times, 153 )
		_build_sysnum_enum_( uname, 160 )
		_build_sysnum_enum_( gettimeofday, 169 )
		_build_sysnum_enum_( getpid, 172 )
		_build_sysnum_enum_( getppid, 173 )
		_build_sysnum_enum_( brk, 214 )
		_build_sysnum_enum_( munmap, 215 )
		_build_sysnum_enum_( fork, 220 )
		_build_sysnum_enum_( exec, 221 )
		_build_sysnum_enum_( mmap, 222 )
		_build_sysnum_enum_( wait, 260 )
		_build_sysnum_enum_( statx, 291 )
		_build_sysnum_enum_( poweroff, 2024 )
	};
#undef _build_sysnum_enum_

} // namespace  syscall

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

#define _build_sysnum_enum_(name, number) SYS_##name = number

	enum SyscallNumber
	{
		_build_sysnum_enum_(getcwd, 17),
		_build_sysnum_enum_(dup, 23),
		_build_sysnum_enum_(dup2, 24),
		_build_sysnum_enum_(fcntl, 25),
		_build_sysnum_enum_(ioctl, 29),
		_build_sysnum_enum_(mkdir, 34),
		_build_sysnum_enum_(unlinkat, 35),
		_build_sysnum_enum_(umount, 39),
		_build_sysnum_enum_(mount, 40),
		_build_sysnum_enum_(statfs, 43),
		_build_sysnum_enum_(faccessat, 48),
		_build_sysnum_enum_(chdir, 49),
		_build_sysnum_enum_(openat, 56),
		_build_sysnum_enum_(close, 57),
		_build_sysnum_enum_(pipe, 59),
		_build_sysnum_enum_(getdents, 61),
		_build_sysnum_enum_(lseek, 62),
		_build_sysnum_enum_(read, 63),
		_build_sysnum_enum_(write, 64),
		_build_sysnum_enum_(sendfile, 71),
		_build_sysnum_enum_(ppoll, 73),
		_build_sysnum_enum_(splice, 76),
		_build_sysnum_enum_(readlinkat, 78),
		_build_sysnum_enum_(fstat, 80),
		_build_sysnum_enum_( utimensat, 88),
		_build_sysnum_enum_(exit, 93),
		_build_sysnum_enum_(exit_group, 94),
		_build_sysnum_enum_(set_tid_address, 96),
		_build_sysnum_enum_(set_robust_list, 99),
		_build_sysnum_enum_(sleep, 101),
		_build_sysnum_enum_(clock_gettime, 113),
		_build_sysnum_enum_(nanosleep, 115),
		_build_sysnum_enum_(syslog, 116),
		_build_sysnum_enum_(sched_yield, 124),
		_build_sysnum_enum_(sigaction, 134),
		_build_sysnum_enum_(sigprocmask, 135),
		_build_sysnum_enum_(setgid, 144),
		_build_sysnum_enum_(setuid, 146),
		_build_sysnum_enum_(times, 153),
		_build_sysnum_enum_(setpgid, 154),
		_build_sysnum_enum_(getpgid, 155),
		_build_sysnum_enum_(uname, 160),
		_build_sysnum_enum_(getrusage, 165),
		_build_sysnum_enum_(gettimeofday, 169),
		_build_sysnum_enum_(getpid, 172),
		_build_sysnum_enum_(getppid, 173),
		_build_sysnum_enum_(getuid, 174),
		_build_sysnum_enum_(geteuid, 175),
		_build_sysnum_enum_(getgid, 176),
		_build_sysnum_enum_(sysinfo,179),
		_build_sysnum_enum_(brk, 214),
		_build_sysnum_enum_(munmap, 215),
		_build_sysnum_enum_(fork, 220),
		_build_sysnum_enum_(execve, 221),
		_build_sysnum_enum_(mmap, 222),
		_build_sysnum_enum_(mprotect, 226),
		_build_sysnum_enum_(wait, 260),
		_build_sysnum_enum_(prlimit64, 261),
		_build_sysnum_enum_(getrandom, 278),
		_build_sysnum_enum_(statx, 291),
		_build_sysnum_enum_(poweroff, 2024),
	};

#undef _build_sysnum_enum_

} // namespace  syscall

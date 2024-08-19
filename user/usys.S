//
// Created by Li shuang ( pseudonym ) on 2024-05-21 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "usys_macro.inc"

.section .user.init

GEN_USER_SYSCALL getcwd 17
GEN_USER_SYSCALL dup 23
GEN_USER_SYSCALL dup2 24
GEN_USER_SYSCALL unlinkat 35
GEN_USER_SYSCALL chdir 49
GEN_USER_SYSCALL openat 56
GEN_USER_SYSCALL close 57
GEN_USER_SYSCALL pipe 59
GEN_USER_SYSCALL read 63
GEN_USER_SYSCALL write 64
GEN_USER_SYSCALL fstat 80
GEN_USER_SYSCALL exit 93
GEN_USER_SYSCALL sleep 101
GEN_USER_SYSCALL sched_yield 124
GEN_USER_SYSCALL gettimeofday 169
GEN_USER_SYSCALL getpid 172
GEN_USER_SYSCALL getppid 173
GEN_USER_SYSCALL brk 214
GEN_USER_SYSCALL clone 220
GEN_USER_SYSCALL execve 221
GEN_USER_SYSCALL wait 260
GEN_USER_SYSCALL poweroff 2024


.global fork
fork:
	li.w	$a7, 220
	li.w	$a0, 17
	move	$a1, $zero
	syscall	0
	ret
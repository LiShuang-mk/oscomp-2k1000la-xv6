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
GEN_USER_SYSCALL write 64
GEN_USER_SYSCALL exit 93
GEN_USER_SYSCALL getpid 172
GEN_USER_SYSCALL getppid 173
GEN_USER_SYSCALL brk 214
GEN_USER_SYSCALL fork 220
GEN_USER_SYSCALL execv 221
GEN_USER_SYSCALL wait 260
GEN_USER_SYSCALL poweroff 2024
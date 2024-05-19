//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define _MAX_SYSCALL_NUM_ 512

typedef unsigned long uint64_t;

extern uint64_t( *syscall_funcs[ _MAX_SYSCALL_NUM_ ] )( void );

#endif // syscall.h
//
// Created by Li Shuang ( pseudonym ) on 2024-05-21 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#ifndef _USYSCALL_H_
#define _USYSCALL_H_

extern int write( int fd, const void *buf, int len );
extern int exit( int n );
extern int fork( void );
extern int getpid( void );
extern int getppid( void );
extern int brk( int n );
extern int execv( const char *path, char * argv[] );
extern int wait( int pid, int *state );
extern int poweroff();

#endif
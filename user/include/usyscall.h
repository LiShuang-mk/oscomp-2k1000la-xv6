//
// Created by Li Shuang ( pseudonym ) on 2024-05-21 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#ifndef _USYSCALL_H_
#define _USYSCALL_H_

extern int read( int fd, void *buf, int len );
extern int write( int fd, const void *buf, int len );
extern int exit( int n );
extern int clone( unsigned int flags, unsigned long sp );
extern int fork( void );
extern int getpid( void );
extern int getppid( void );
extern int brk( int n );
extern int execve( const char *path, const char * argv[], const char * envp[] );
extern int wait( int pid, int *state );
extern int poweroff();
extern int dup( int fd );
extern int dup2( int oldfd, int newfd );
extern char * getcwd( char *buf, unsigned int size );
extern int sleep( int n );
extern int openat( int dirfd, const char *path_name, int flags, int mode );
extern int close( int fd );
extern int unlinkat( int fd, char *fname, int flags );
extern int pipe( int *fd );
extern int chdir( const char *path );
#endif
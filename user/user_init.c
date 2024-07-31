//
// Created by Li Shuang ( pseudonym ) on 2024-05-18 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "usyscall.h"

char u_init_stack[ 4096 ] __attribute__( ( section( ".user.init.stack" ) ) );

int init_main( void ) __attribute__( ( section( ".user.init" ) ) );
static void printint( int xx, int base, int sign ) __attribute__( ( section( ".user.init" ) ) );

__attribute__( ( section( ".user.init.data" ) ) ) const char str[] = "\nHello World\n\n\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char errstr[] = "fork fail\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char parent_str[] = "parent write. fork pid is ";
__attribute__( ( section( ".user.init.data" ) ) ) const char child_str[] = "child write\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_fail_str[] = "exec fail\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char wait_success[] = "  wait success\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char wait_fail[] = "wait fail\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char sleep_success[] = "sleep_success\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char print_int_error[] = "printint error\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char to_open_file[] = "text.txt";
__attribute__( ( section( ".user.init.data" ) ) ) const char read_fail[] = "read fail\n";


__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_echo[] = "write";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_fork[] = "fork";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_exit[] = "exit";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_wait[] = "wait";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_getpid[] = "getpid";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_getppid[] = "getppid";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_dup[] = "dup";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_dup2[] = "dup2";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_execve[] = "execve";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_getcwd[] = "getcwd";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_gettimeofday[] = "gettimeofday";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_yield[] = "yield";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_sleep[] = "sleep";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_times[] = "times";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_clone[] = "clone";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_brk[] = "brk";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_uname[] = "uname";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_waitpid[] = "waitpid";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_open[] = "open";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_fstat[] = "fstat";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_openat[] = "openat";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_close[] = "close";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_read[] = "read";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_getdents[] = "getdents";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_mkdir[] = "mkdir_";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_chdir[] = "chdir";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_mount[] = "mount";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_umount[] = "umount";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_mmap[] = "mmap";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_munmap[] = "munmap";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_unlinkat[] = "unlink";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_test_pipe[] = "pipe";

__attribute__( ( section( ".user.init.data" ) ) ) const char exec_busybox[] = "busybox";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_busybox_unstrp[] = "busybox_unstrp";
__attribute__( ( section( ".user.init.data" ) ) ) const char busybox_name[] = "busybox";
__attribute__( ( section( ".user.init.data" ) ) ) const char sh_name[] = "ash";
__attribute__( ( section( ".user.init.data" ) ) ) const char echo_name[] = "echo";
__attribute__( ( section( ".user.init.data" ) ) ) const char cat_name[] = "cat";
__attribute__( ( section( ".user.init.data" ) ) ) const char hello_busybox_str[] = "hello, busybox!\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char busybox_testcode_str[] = "/mnt/sdcard/busybox_testcode.sh";
__attribute__( ( section( ".user.init.data" ) ) ) const char test_sh_str[] = "/mnt/sdcard/test.sh";
__attribute__( ( section( ".user.init.data" ) ) ) const char busybox_path[] = "/mnt/sdcard/";
// __attribute__( ( section( ".user.init.data.p" ) ) ) const char *bb_sh[] = { sh_name, 0 };

__attribute__( ( section( ".user.init.data" ) ) ) const char exec_time_test[] = "time-test";

__attribute__( ( section( ".user.init.data" ) ) ) const char exec_libcbench[] = "libc-bench";


__attribute__( ( section( ".user.init.data" ) ) ) const char digits[] = "0123456789abcdef";

__attribute__( ( __unused__ ) ) static void printint( int xx, int base, int sign )
{
	char buf[ 16 + 1 ];
	int i;
	unsigned int x;

	if ( sign && ( sign = xx < 0 ) )
		x = -xx;
	else
		x = xx;

	buf[ 16 ] = 0;
	i = 15;
	do
	{
		buf[ i-- ] = digits[ x % base ];
	}
	while ( ( x /= base ) != 0 );

	if ( sign )
		buf[ i-- ] = '-';
	i++;
	if ( i < 0 )
		write( 1, print_int_error, sizeof( print_int_error ) );
	write( 1, buf + i, 16 - i );
}

int init_main( void )
{
	write( 1, str, sizeof( str ) - 1 );

	__attribute__( ( __unused__ ) ) int pid;

	__attribute__( ( __unused__ ) ) const char *bb_sh[ 4 ] = { 0 };

	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		chdir( busybox_path );
		bb_sh[ 0 ] = sh_name;
		bb_sh[ 1 ] = test_sh_str;
		bb_sh[ 2 ] = 0;
		bb_sh[ 3 ] = 0;
		if ( execv( exec_time_test, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		write( 1, "exec busybox sh fail", 21 );
	}

	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		chdir( busybox_path );
		bb_sh[ 0 ] = sh_name;
		bb_sh[ 1 ] = busybox_testcode_str;
		bb_sh[ 2 ] = 0;
		bb_sh[ 3 ] = 0;
		if ( execv( exec_busybox, bb_sh ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		write( 1, "exec busybox sh fail", 21 );
	}

	while ( 1 );

#ifndef OS_DEBUG

	// ======== test clone ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_clone, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test fork ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_fork, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test echo ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_echo, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test read ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_read, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test exit ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_exit, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test wait ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_wait, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test getpid ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_getpid, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test getppid ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_getppid, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test dup ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_dup, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test dup2 ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_dup2, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test getcwd ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_getcwd, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test gettimeofday ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_gettimeofday, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test yield ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_yield, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test sleep ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_sleep, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test times ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_times, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test waitpid ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_waitpid, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 4 );
	}
	else
	{
		int child_exit_state = -100;
		int waitpid;
		// if ( wait( -1, &child_exit_state ) < 0 )
		if ( ( waitpid = wait( pid, &child_exit_state ) ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// {
		// 	write( 1, wait_success, sizeof( wait_success ) );
		// 	printint( pid, 10, 1 );
		// 	write( 1, wait_success, sizeof( wait_success ) );
		// 	printint( waitpid, 10, 1 );
		// 	write( 1, wait_success, sizeof( wait_success ) );
		// 	printint( child_exit_state, 10, 1 );
		// 	write( 1, wait_success, sizeof( wait_success ) );
		// }
	}

	// ======== test brk ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_brk, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test uname ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_uname, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test open ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_open, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ========== test fstat ==========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_fstat, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test close ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_close, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test openat ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_openat, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test getdents ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_getdents, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test mkdir ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_mkdir, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test chdir ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_chdir, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}


	// ======== test execve ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_execve, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test mount ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_mount, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test umount ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_umount, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test mmap ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_mmap, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test unmmap ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_munmap, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}


	// ======== test unlinkat ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_unlinkat, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

	// ======== test pipe ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_pipe, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

#else	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DEBUG

	// ======== test read ========
	pid = fork();
	if ( pid < 0 )
	{
		write( 1, errstr, sizeof( errstr ) );
	}
	else if ( pid == 0 )
	{
		if ( execv( exec_test_read, 0 ) < 0 )
		{
			write( 1, exec_fail_str, sizeof( exec_fail_str ) );
		}
		exit( 0 );
	}
	else
	{
		int child_exit_state = -100;
		if ( wait( -1, &child_exit_state ) < 0 )
			write( 1, wait_fail, sizeof( wait_fail ) );
		// else
		// 	write( 1, wait_success, sizeof( wait_success ) );
	}

#endif



	for ( long int i = 0; i < 1000000000; i++ )
	{

	}

#ifndef OS_DEBUG
	// power off
	poweroff();
#endif

	while ( 1 );
}
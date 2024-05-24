//
// Created by Li Shuang ( pseudonym ) on 2024-05-18 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "usyscall.h"

char u_init_stack[ 2048 ] __attribute__( ( section( ".user.init.stack" ) ) );

int init_main( void ) __attribute__( ( section( ".user.init" ) ) );

__attribute__( ( section( ".user.init.data" ) ) ) const char str[] = "Hello World\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char errstr[] = "fork fail\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char parent_str[] = "parent write. fork pid is ";
__attribute__( ( section( ".user.init.data" ) ) ) const char child_str[] = "child write\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char exec_fail_str[] = "exec fail\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char wait_success[] = "wait success\n";
__attribute__( ( section( ".user.init.data" ) ) ) const char wait_fail[] = "wait fail\n";


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


int init_main( void )
{
	write( 1, str, sizeof( str ) );

	int pid;

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

#ifndef OS_DEBUG
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
		else
			write( 1, wait_success, sizeof( wait_success ) );
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



#endif




	for ( int i = 0; i < 1000000000; i++ )
	{

	}

#ifndef OS_DEBUG
	// power off
	// poweroff();
#endif

	while ( 1 );
}
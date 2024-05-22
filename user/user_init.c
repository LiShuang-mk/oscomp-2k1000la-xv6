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


int init_main( void )
{
	// asm volatile(
	// 	"li.d $a0, 1\n"
	// 	"add.d $a1, $zero, %0\n"
	// 	"li.d $a2, %1\n"
	// 	"li.d $a7, 64\n"
	// 	"syscall 0"
	// 	:: "r"( str ),
	// 	"i"( sizeof( str ) )
	// 	);
	// write( 1, str, sizeof( str ) );
	
	// int pid = fork();
	// if ( pid < 0 )
	// {
	// 	write( 1, errstr, sizeof( errstr ) );
	// }
	// else if ( pid == 0 )
	// {
	// 	write( 1, child_str, sizeof( child_str ) );
	// 	int ppid_ = getppid();
	// 	char ppid = ( char ) ppid_ + '0';
	// 	write( 1, &ppid, sizeof( ppid ) );
	// 	int pid_  = getpid();
	// 	char pid = ( char ) pid_ + '0';
	// 	write( 1, &pid, sizeof( pid ) );
	// }
	// else
	// {
	// 	//int ppid_ = getppid();
	// 	int brk_ = brk( 10000 );
		char brk = ( char ) brk_ + '0';
		write( 1, &brk, sizeof( brk ) );
		//char ppid = ( char ) ppid_ + '0';
	// 	//write( 1, &ppid, sizeof( ppid ) );
	// 	//int pid_  = getpid();
	// 	//char pid = ( char ) pid_ + '0';
	// 	//write( 1, &pid, sizeof( pid ) );
	// 	write( 1, parent_str, sizeof( parent_str ) );
	// 	char ch = ( char ) pid + '0';
	// 	write( 1, &ch, sizeof( ch ) );
	// 	ch = '\n';
	// 	write( 1, &ch, sizeof( ch ) );
	// }

	while ( 1 );
}
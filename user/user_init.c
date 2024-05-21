//
// Created by Li Shuang ( pseudonym ) on 2024-05-18 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "syscall.h"

char u_init_stack[ 2048 ] __attribute__( ( section( ".user.init.stack" ) ) );

int init_main( void ) __attribute__( ( section( ".user.init" ) ) );

int init_main( void )
{
	const char str[] = "Hello World\n";
	// asm volatile(
	// 	"li.d $a0, 1\n"
	// 	"add.d $a1, $zero, %0\n"
	// 	"li.d $a2, %1\n"
	// 	"li.d $a7, 64\n"
	// 	"syscall 0"
	// 	:: "r"( str ),
	// 	"i"( sizeof( str ) )
	// 	);
	write( 1, str, sizeof( str ) );
	while ( 1 );
}
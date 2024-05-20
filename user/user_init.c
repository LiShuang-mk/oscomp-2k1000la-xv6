//
// Created by Li Shuang ( pseudonym ) on 2024-05-18 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

char u_init_stack[ 2048 ] __attribute__( ( section( ".user.init.stack" ) ) );

int init_main( void ) __attribute__( ( section( ".user.init" ) ) );

int init_main( void )
{
	while ( 1 );
	asm volatile(
		"li.d $a7, 99\n"
		"syscall 0"
	);
}
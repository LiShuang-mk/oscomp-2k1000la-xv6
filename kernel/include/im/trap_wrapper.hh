//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

extern "C" {
	extern void _wrp_kernel_trap( void );
	extern void _wrp_machine_trap( void );
	extern void _wrp_user_trap( void );
	extern void _wrp_user_trap_ret( void );
}
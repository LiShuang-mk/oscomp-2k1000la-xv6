//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "klib/printer.hh"

void printf( const char *fmt, ... );
void *memset( void *dst, int c, uint n );
// void panic( const char *s );

#define log_panic(panic_info) kernellib::k_printer.panic( __FILE__, __LINE__, panic_info );
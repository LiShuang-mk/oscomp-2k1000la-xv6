//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "hal/uart.hh"

class Console
{
private:
	smp::Lock _lock;
	UartManager _uart_m;
	const int _backspace = 0x100;
public:
	Console();
	void init( const char *name );
	void putc( char c );
};

extern Console k_console;
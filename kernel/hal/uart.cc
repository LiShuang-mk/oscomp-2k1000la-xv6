//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/uart.hh"
#include "hal/cpu.hh"
#include "klib/printer.hh"

void UartManager::init( uint64 u_addr )
{
	_uart_base = u_addr;

	_write_reg( UartReg::IER, 0x0 );
	_write_reg( UartReg::LCR, UartLCR::access_baud );
	_write_reg( UartBaud::low_8_bit, 0x03 );
	_write_reg( UartBaud::high_8_bit, 0x00 );
	_write_reg( UartReg::LCR, UartLCR::use_8_bits );
	_write_reg( UartReg::FCR, UartFCR::enable | UartFCR::clear );
	_write_reg( UartReg::IER, UartIER::rx_en );
	
	_lock.init( "uart" );
	tx_w = tx_r = 0;
}

void UartManager::putc_sync( int c )
{
	loongarch::Cpu::push_intr_off();
	if ( kernellib::k_printer.is_panic() )
		while ( 1 );
	while ( ( _read_reg( UartReg::LSR ) & UartLSR::tx_idle ) == 0 )
		;
	_write_reg( UartReg::THR, c );
	loongarch::Cpu::pop_intr_off();
}

void UartManager::_write_reg( uint32 reg, uint8 data )
{
	*( volatile unsigned char * ) ( _uart_base + reg )
		= data;
}

uint8 UartManager::_read_reg( uint32 reg )
{
	return *( volatile unsigned char * ) ( _uart_base + reg );
}
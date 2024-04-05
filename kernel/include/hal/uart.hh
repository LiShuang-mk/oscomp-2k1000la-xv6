//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

class UartManager
{
private:
	uint64 _uart_base;
	smp::Lock _lock;
	static const int _buf_size = 32;
	char _buf[ _buf_size ];
	uint64 tx_w;
	uint64 tx_r;

public:
	UartManager() {}
	void init( uint64 u_addr );
	void putc_sync( int c );

private:
	enum UartReg
	{
		RHR = 0,
		THR = 0,
		IER = 1,
		FCR = 2,
		ISR = 2,
		LCR = 3,
		LSR = 5,
	};
	enum UartIER
	{
		rx_en = 0x1 << 0,
		tx_en = 0x1 << 1,
	};
	enum UartFCR
	{
		enable = 0x1 << 0,
		rx_clear = 0x1 << 1,
		tx_clear = 0x1 << 2,
		clear = 0x3 << 1
	};
	enum UartLCR
	{
		use_8_bits = 0x3 << 0,
		access_baud = 0x1 << 7,
	};
	enum UartLSR
	{
		rx_ready = 0x1 << 0,
		tx_idle = 0x1 << 5,
	};
	enum UartBaud
	{
		low_8_bit = 0,
		high_8_bit = 1
	};
	void _write_reg( uint32 reg, uint8 data );
	uint8 _read_reg( uint32 reg );
};
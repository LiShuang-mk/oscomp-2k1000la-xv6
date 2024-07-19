//
// Created by Li Shuang ( pseudonym ) on 2024-06-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "uart/uart_ns16550.hh"
#include "hsai_log.hh"

namespace hsai
{
	UartNs16550::UartNs16550() {}
	UartNs16550::UartNs16550( void * reg_base ) : _reg_base( ( u64 ) reg_base ) {}

	void UartNs16550::init()
	{
		// disable interrrupt
		_write_reg( IER, 0x00 );

		// set baudrate
		regLCR * lcr = ( regLCR* ) ( _reg_base + LCR );
		lcr->divisor_access = 1;
		_write_reg( DLL, 0x03 );
		_write_reg( DLM, 0x00 );
		lcr->divisor_access = 0;

		// word length 8 bits
		lcr->word_length = regLCR::WordLen::bits_8;

		// reset and enable FIFO
		regFCR * fcr = ( regFCR* ) ( _reg_base + FCR );
		fcr->tx_fifo_reset = fcr->rx_fifo_reset = 1;
		fcr->enable = 1;

		// enable interrupt
		regIER * ier = ( regIER* ) ( _reg_base + IER );
		ier->data_ready = 1;

		_lock.init( "UART" );
	}

	int UartNs16550::put_char_sync( u8 c )
	{
		_lock.acquire();
		regLSR * lsr = ( regLSR* ) ( _reg_base + LSR );
		while ( lsr->thr_empty == 0 )
			;
		_write_reg( THR, ( u8 ) c );
		_lock.release();
		return 0;
	}

	int UartNs16550::put_char( u8 c )
	{
		hsai_panic( "put char not implement" );
		return -1;
	}

	uint8 UartNs16550::get_char_sync()
	{
		return uint8();
	}

	uint8 UartNs16550::get_char()
	{
		return uint8();
	}

	int UartNs16550::handle_intr()
	{
		hsai_panic( "handle intr not implement" );
		return -1;
	}

} // namespace hsai

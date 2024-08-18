//
// Created by Li Shuang ( pseudonym ) on 2024-06-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "uart/uart_ns16550.hh"
#include "process_interface.hh"
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
		_write_reg( DLL, 68 );
		_write_reg( DLM, 0x00 );
		lcr->divisor_access = 0;

		// word length 8 bits
		lcr->word_length = regLCR::WordLen::bits_8;

		// reset and enable FIFO
		regFCR * fcr = ( regFCR* ) ( _reg_base + FCR );
		fcr->tx_fifo_reset = fcr->rx_fifo_reset = 1;
		fcr->enable = 1;
		fcr->rx_fifo_level = 0;

		// enable interrupt
		regIER * ier = ( regIER* ) ( _reg_base + IER );
		ier->data_ready = 1;
		// ier->thr_empty = 1;

		_lock.init( "UART" );
	}

	int UartNs16550::put_char_sync( u8 c )
	{
		_lock.acquire();
		volatile regLSR * lsr = ( volatile regLSR* ) ( _reg_base + LSR );
		while ( lsr->thr_empty == 0 )
			;
		_write_reg( THR, ( u8 ) c );
		_lock.release();
		return 0;
	}

	int UartNs16550::put_char( u8 c )
	{
		_lock.acquire();
		while ( 1 )
		{
			if ( _wr_idx == _rd_idx + _buf_size )
			{
				// buffer is full.
				// wait for uartstart() to open up space in the buffer.
				sleep_at( &_rd_idx, _lock );
			}
			else
			{
				_buf[ _wr_idx % _buf_size ] = c;
				_wr_idx += 1;
				_start();
				_lock.release();
				return 0;
			}
		}
	}

	int UartNs16550::get_char_sync( u8 * c )
	{
		volatile regLSR * lsr = ( volatile regLSR * ) ( _reg_base + LSR );
		while ( lsr->data_ready == 0 );
		*c = _read_reg( THR );
		return 0;
	}

	int UartNs16550::get_char( u8 * c )
	{
		if ( _read_buffer_empty() )
			return -1;
		else
		{
			*c = _read_buffer_get();
			return 0;
		}
	}

	int UartNs16550::handle_intr()
	{
		u8 c = 0;
		volatile regLSR * lsr = ( volatile regLSR * ) ( _reg_base + LSR );
		while ( !_read_buffer_full() )
		{
			if ( lsr->data_ready == 1 )
			{
				c = _read_reg( THR );
				// hsai_info( "uartns16550 intr: %c", c );
				_read_buffer_put( c );
			}
			else break;
		}

		_lock.acquire();
		_start();
		_lock.release();

		return 0;
	}

	void UartNs16550::_start()
	{
		volatile regLSR * lsr = ( volatile  regLSR * ) ( _reg_base + LSR );
		volatile char * thr = ( volatile char * ) ( _reg_base + THR );
		while ( 1 )
		{
			if ( _wr_idx == _rd_idx )
			{
				// transmit buffer is empty.
				return;
			}

			if ( lsr->thr_empty == 0 )
			{
				// the UART transmit holding register is full,
				// so we cannot give it another byte.
				// it will interrupt when it's ready for a new byte.
				return;
			}

			char c = _buf[ _rd_idx % _buf_size ];
			_rd_idx += 1;

			// maybe uartputc() is waiting for space in the buffer.
			wakeup_at( &_rd_idx );

			*thr = c;
		}
	}

} // namespace hsai

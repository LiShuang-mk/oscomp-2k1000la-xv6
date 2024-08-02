//
// Created by Li Shuang ( pseudonym ) on 2024-06-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

/***********************************************
 * 16550 UART 驱动
 * 芯片参考 http://caro.su/msx/ocm_de1/16550.pdf
 ***********************************************/

#pragma once 

#include "kernel/types.hh"
#include "uart/virtual_uart.hh"
#include "smp/spin_lock.hh"

namespace hsai
{

	class UartNs16550 : public VirtualUartController
	{
	private:
		static constexpr ulong _buf_size = 1024;
		SpinLock _lock;
		u64 _reg_base = 0;

		char _buf[ _buf_size ];
		ulong _wr_idx = 0;
		ulong _rd_idx = 0;

		char _read_buf[ _buf_size ];
		ulong _read_front = 0;
		ulong _read_tail = 0;

	public:
		UartNs16550();
		UartNs16550( void * reg_base );

		virtual void init() override;
		virtual int put_char_sync( u8 c ) override;
		virtual int put_char( u8 c ) override;
		virtual int get_char_sync( u8 * c ) override;
		virtual int get_char( u8 * c ) override;
		virtual int handle_intr() override;

	public:
		virtual bool read_ready() override { return !_read_buffer_empty(); }
		virtual bool write_ready() override { return !_read_buffer_full(); }

	private:
		void _start();

	private:
		// 内部寄存器偏移
		enum RegOffset
		{
			/* General Register Set */

			RHR = 0,				// Receiver Holding Register
			THR = 0,				// Transmitter Holding Register
			IER = 1,				// Interrupt Enable Register
			FCR = 2,				// FIFO Control Register
			ISR = 2,				// Interrupt Status Regsiter
			LCR = 3,				// Line Control Register
			MCR = 4,				// Modem Control Register
			LSR = 5,				// Line Status Register
			MSR = 6,				// Modem Status Regsiter
			SPR = 7,				// Scratch Pad Register

			/* Division Register Set */

			DLL = 0,				// Divisor Latch, Least significant byte
			DLM = 1,				// Divisor Latch, Most significant byte
			PSD = 5,				// PreScaler Division
		};
		struct regIER
		{
			u8 data_ready : 1;
			u8 thr_empty : 1;
			u8 lsr_int : 1;
			u8 modem_int : 1;
			u8 _rev : 2;
			u8 dma_rx : 1;
			u8 dma_tx : 1;
		};
		static_assert( sizeof( regIER ) == 1 );
		struct regFCR
		{
			u8 enable : 1;
			u8 rx_fifo_reset : 1;
			u8 tx_fifo_reset : 1;
			u8 dma_mode : 1;
			u8 dma_end_enable : 1;
			u8 _rev : 1;
			u8 rx_fifo_level : 2;
		};
		static_assert( sizeof( regFCR ) == 1 );
		struct regISR
		{
			u8 intr_status : 1;
			u8 intr_code : 3;
			u8 dma_rx_end : 1;
			u8 dma_tx_end : 1;
			u8 fifo_enabled : 2;
		};
		static_assert( sizeof( regISR ) == 1 );
		struct regLCR
		{
			enum WordLen : u8
			{
				bits_5 = 0,
				bits_6 = 1,
				bits_7 = 2,
				bits_8 = 3,
			};
			u8 word_length : 2;
			u8 stop_bits : 1;
			u8 parity_enbale : 1;
			u8 even_parity : 1;
			u8 force_parity : 1;
			u8 set_break : 1;
			u8 divisor_access : 1;
		};
		static_assert( sizeof( regLCR ) == 1 );
		struct regMCR
		{
			u8 dtr : 1;
			u8 rts : 1;
			u8 out1 : 1;
			u8 out2 : 1;
			u8 loop_back : 1;
			u8 _rev : 3;
		};
		static_assert( sizeof( regMCR ) == 1 );
		struct regLSR
		{
			u8 data_ready : 1;
			u8 overrun_error : 1;
			u8 parity_error : 1;
			u8 framing_error : 1;
			u8 break_intr : 1;
			u8 thr_empty : 1;
			u8 transmit_empty : 1;
			u8 fifo_data_error : 1;
		};
		static_assert( sizeof( regLSR ) == 1 );
		struct regMSR
		{
			u8 delta_cts : 1;
			u8 delta_dsr : 1;
			u8 trail_edge_ri : 1;
			u8 delta_cd : 1;
			u8 cts : 1;
			u8 dst : 1;
			u8 ri : 1;
			u8 cd : 1;
		};
		static_assert( sizeof( regMSR ) == 1 );

		struct regPSD
		{
			u8 precaler_division : 4;
			u8 _rev : 4;
		};
		static_assert( sizeof( regPSD ) == 1 );

	private:
		constexpr void _write_reg( RegOffset reg, u8 data ) { ( ( volatile u8* ) _reg_base )[ ( int ) reg ] = data; }
		constexpr u8 _read_reg( RegOffset reg ) { return ( ( volatile u8* ) _reg_base )[ ( int ) reg ]; }
		constexpr bool _read_buffer_full() { return _read_front == _read_tail + _buf_size; }
		constexpr bool _read_buffer_empty() { return _read_front == _read_tail; }
		constexpr void _read_buffer_put( char c ) { _read_buf[ _read_front % _buf_size ] = c; _read_front++; }
		constexpr char _read_buffer_get() { _read_tail++; return _read_buf[ ( _read_tail - 1 ) % _buf_size ]; }
	};

} // namespace hsai

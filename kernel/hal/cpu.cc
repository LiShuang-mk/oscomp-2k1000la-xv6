//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/cpu.hh"

namespace loongarch
{
	Cpu k_cpus[ NUMCPU ];

// ---- public: 

	inline int Cpu::get_intr_stat()
	{
		uint32 x = _read_crmd();
		return ( x & csr::crmd::ie_m ) != 0;
	}

	Cpu *Cpu::get_cpu()
	{
		uint64 x = read_tp();
		return &k_cpus[ x ];
	}

	void Cpu::push_intr_off()
	{
		int old = get_intr_stat();

		_intr_off();
		Cpu *c_cpu = get_cpu();
		if ( c_cpu->_num_off == 0 )
			c_cpu->_int_ena = old;
		c_cpu->_num_off += 1;
	}

	void Cpu::pop_intr_off()
	{
		Cpu *c_cpu = get_cpu();
		if ( get_intr_stat() )
			while ( 1 );// panic: pop_off - interruptible 
		if ( c_cpu->_num_off < 1 )
			while ( 1 );// panic: pop_off - none to pop off 
		c_cpu->_num_off -= 1;
		if ( c_cpu->_num_off == 1 && c_cpu->_int_ena )
			_intr_on();
	}

// ---- private:

	// inline uint32 Cpu::_read_csr( uint32 x )
	// {
	// 	uint32 v;

	// 	asm volatile(
	// 		"csrrd %0, %1"
	// 		: "=r" ( v )
	// 		: "r" ( x )
	// 		);
	// 	return v;
	// }

	// inline void Cpu::_write_csr( uint32 x, uint32 v )
	// {
	// 	asm volatile(
	// 		"csrwr %0, %1"
	// 		:: "r" ( v ),
	// 		"r" ( x )
	// 		);
	// }

	inline uint32 Cpu::_read_crmd()
	{
		uint32 v;

		asm volatile(
			"csrrd %0, %1"
			: "=r" ( v )
			: "i" ( csr::CsrAddr::crmd )
			);
		return v;
	}

	inline void Cpu::_write_crmd( uint32 v )
	{
		asm volatile(
			"csrwr %0, %1"
			:: "r" ( v ),
			"i" ( csr::CsrAddr::crmd )
			);
	}

	void Cpu::_intr_on()
	{
		_write_crmd( _read_crmd() | csr::crmd::ie_m );
	}

	void Cpu::_intr_off()
	{
		_write_crmd( _read_crmd() & ~csr::crmd::ie_m );
	}
} // namespace loongarch

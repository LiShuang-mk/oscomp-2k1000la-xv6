//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/cpu.hh"
#include "klib/common.hh"
#include "pm/process.hh"
#include "pm/trap_frame.hh"
namespace loongarch
{
	Cpu k_cpus[ NUMCPU ];

// ---- public: 

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
			log_panic("pop intr off - interruptible");
		if ( c_cpu->_num_off < 1 )
			log_panic("pop intr off - none to pop off");
		c_cpu->_num_off -= 1;
		if ( c_cpu->_num_off == 0 && c_cpu->_int_ena )
			_intr_on();
	}

	void Cpu::euen_float()
	{
		uint64 tmp = read_csr( csr::CsrAddr::euen );
		write_csr( csr::CsrAddr::euen, tmp | 1 );
	}

// ---- private:

	uint64 Cpu::read_csr( csr::CsrAddr r )
	{
		return csr::_read_csr_( r );
	}

	void Cpu::write_csr( csr::CsrAddr r, uint64 d )
	{
		return csr::_write_csr_( r, d );
	}

	void Cpu::_intr_on()
	{
		uint64 tmp = read_csr( csr::CsrAddr::crmd );
		write_csr( csr::CsrAddr::crmd, tmp | csr::Crmd::ie_m );
	}

	void Cpu::_intr_off()
	{
		uint64 tmp = read_csr( csr::CsrAddr::crmd );
		write_csr( csr::CsrAddr::crmd, tmp & ~csr::Crmd::ie_m );
	}
} // namespace loongarch

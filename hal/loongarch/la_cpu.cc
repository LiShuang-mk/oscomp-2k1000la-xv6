//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "la_cpu.hh"
#include "context.hh"

#include <hsai_global.hh>

namespace loongarch
{
	static Cpu k_la_cpus[ NUMCPU ];
	static Context k_la_cpu_context[ NUMCPU ];

	Cpu::Cpu() : VirtualCpu()
	{
		_context = &k_la_cpu_context[ get_cpu_id() ];
	}

	void Cpu::_interrupt_on()
	{
		uint64 tmp = read_csr( csr::CsrAddr::crmd );
		write_csr( csr::CsrAddr::crmd, tmp | csr::crmd_ie_m );
	}

	void Cpu::_interrupt_off()
	{
		uint64 tmp = read_csr( csr::CsrAddr::crmd );
		write_csr( csr::CsrAddr::crmd, tmp & ~csr::crmd_ie_m );
	}

	Cpu * Cpu::get_la_cpu()
	{
		return ( Cpu * ) hsai::get_cpu();
	}

	uint Cpu::get_cpu_id()
	{
		uint64 x;
		asm volatile( "addi.d %0, $tp, 0" : "=r" ( x ) );
		return x;
	}

	int Cpu::is_interruptible()
	{
		uint64 x = read_csr( csr::CsrAddr::crmd );
		return ( x & csr::crmd_ie_m ) != 0;
	}

	uint64 Cpu::read_csr( csr::CsrAddr r )
	{
		return csr::_read_csr_( r );
	}

	void Cpu::write_csr( csr::CsrAddr r, uint64 d )
	{
		csr::_write_csr_( r, d );
	}

} // namespace loongarch

extern "C" {
	void _cpu_init()
	{
		int i;
		asm volatile( "addi.d %0, $tp, 0" : "=r" ( i ) );
		new ( &loongarch::k_la_cpus[ i ] ) loongarch::Cpu;
		loongarch::Cpu::register_cpu( loongarch::k_la_cpus + i, i );
		
	}
}

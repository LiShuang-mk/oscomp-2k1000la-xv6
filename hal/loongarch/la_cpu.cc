//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "la_cpu.hh"

namespace loongarch
{
	Cpu k_la_cpus[ NUMCPU ];

	void cpu_init()
	{
		for ( int i = 0; i < NUMCPU; i++ )
		{
			new ( &k_la_cpus[ i ] ) Cpu;
			hsai::register_cpu( k_la_cpus + i, i );
		}

	}

	void Cpu::_interrupt_on()
	{

	}

	void Cpu::_interrupt_off()
	{

	}

	uint Cpu::get_cpu_id()
	{
		uint64 x;
		asm volatile( "addi.d %0, $tp, 0" : "=r" ( x ) );
		return x;
	}

	int Cpu::is_interruptible()
	{
		return 0;
	}

} // namespace loongarch

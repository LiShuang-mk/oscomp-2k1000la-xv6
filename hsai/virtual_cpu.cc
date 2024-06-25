//
// Created by Li Shuang ( pseudonym ) on 2024-06-15
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "virtual_cpu.hh"
#include "hsai_log.hh"

namespace hsai
{
	VirtualCpu * k_cpus[ NUMCPU ] = { nullptr };

	int register_cpu( virtual * p_cpu, int cpu_id )
	{
		if ( cpu_id < 0 || cpu_id >= NUMCPU )
			return -1;
		if ( p_cpu == nullptr )
			return -2;
		k_cpus[ cpu_id ] = p_cpu;
		return cpu_id;
	}

	VirtualCpu::VirtualCpu()
		: _num_off( 0 )
		, _int_ena( 0 )
	{
	}

	void VirtualCpu::push_interrupt_off()
	{
		int old = is_interruptible();
		_interrupt_off();
		if ( _num_off == 0 )
			_int_ena = old;
		_num_off++;
	}

	void VirtualCpu::pop_intterupt_off()
	{
		/// TODO: log out panic : pop_off interruptible
		if ( is_interruptible() )
			hsai_panic( "pop-off : interruptible" );

		/// TODO: log out panic : pop_off no off
		if ( _num_off < 1 )
			hsai_panic( "pop-off : no off" );

		_num_off--;
		if ( _num_off == 0 && _int_ena )
			_interrupt_on();
	}


} // namespace hsai

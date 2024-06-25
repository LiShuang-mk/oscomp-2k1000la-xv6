//
// Created by Li Shuang ( pseudonym ) on 2024-06-15 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

namespace hsai	// hardware service abstract interface
{
	class VirtualCpu
	{
		using uint = unsigned int;

	protected:
		int _num_off;		// number of interrupt's off
		int _int_ena;		// interrupt enable status before push-off()

		virtual void _interrupt_on() = 0;
		virtual void _interrupt_off() = 0;

	public:
		VirtualCpu();

		virtual uint get_cpu_id() = 0;
		virtual int is_interruptible() = 0;

	public:
		int get_num_off() { return _num_off; }
		int get_int_ena() { return _int_ena; }

	public:
		void push_interrupt_off();
		void pop_intterupt_off();
	};

	// constexpr int max_cpu_num = 4;
	extern VirtualCpu * k_cpus[ NUMCPU ];

	int register_cpu( VirtualCpu * p_cpu, int cpu_id );

} // namespace hsai

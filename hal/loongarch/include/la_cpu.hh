//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include <virtual_cpu.hh>

namespace loongarch
{
	class Cpu : public hsai::VirtualCpu
	{
	protected:
		virtual void _interrupt_on() override;
		virtual void _interrupt_off() override;

	public:
		Cpu() {};

		virtual uint get_cpu_id() override;
		virtual int is_interruptible() override;

	};

	extern Cpu k_la_cpus[ NUMCPU ];
	void cpu_init();
	
} // namespace loongarch

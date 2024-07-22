//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "loongarch.hh"

#include <kernel/types.hh>
#include <virtual_cpu.hh>

extern "C" {
	void _cpu_init();
}

namespace loongarch
{
	class Cpu : public hsai::VirtualCpu
	{
	protected:
		virtual void _interrupt_on() override;
		virtual void _interrupt_off() override;

	public:
		Cpu();
		static Cpu * get_la_cpu();

		virtual uint get_cpu_id() override;
		virtual int is_interruptible() override;

		uint64 read_csr( csr::CsrAddr r );
		void write_csr( csr::CsrAddr r, uint64 d );

		void intr_on() { _interrupt_on(); }
		void intr_off() { _interrupt_off(); }
	};

} // namespace loongarch

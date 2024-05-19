//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "hal/cpu.hh"

namespace smp
{
	/// @brief spinlock: each lock only be held by one CPU at the same time. 
	class Lock
	{
	private:
		const char *name;
		uint  locked;
		loongarch::Cpu *cpu;
	public:
		Lock();

		/// @brief init spinlock 
		/// @param name for debugging 
		void init( const char *name );

		/// @brief request for spinlock
		void acquire();

		/// @brief release spinlock
		void release();

		int held();

		inline bool is_held(){
			bool r;
			r = ( locked && cpu == loongarch::Cpu::get_cpu() );
			return r;
		}
	};
} // namespace smp


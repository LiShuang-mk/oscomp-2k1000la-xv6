//
// Created by Li Shuang ( pseudonym ) on 2024-06-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "hsai_types.hh"

#include <atomic>

namespace hsai
{
	class VirtualCpu;

	class SpinLock
	{
	private:
		const char *_name = nullptr;
		std::atomic<VirtualCpu *> _locked = 0;
	public:
		SpinLock();

		/// @brief init spinlock 
		/// @param name for debugging 
		void init( const char *name );

		/// @brief request for spinlock
		void acquire();

		/// @brief release spinlock
		void release();

		bool is_held();
	};

} // namespace hsai

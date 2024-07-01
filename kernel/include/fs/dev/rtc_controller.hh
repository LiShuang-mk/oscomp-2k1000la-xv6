//
// Created by Li Shuang ( pseudonym ) on 2024-05-24 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <smp/spin_lock.hh>

namespace dev
{
	namespace rtc
	{
		/// @brief 在qemu上暂时没有测试成功。这个类留作将来使用
		class RtcController
		{
		private:
			hsai::SpinLock _lock;

		public:
			void init( const char * lock_name );
		};

	} // namespace rtc
	
} // namespace dev

//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "klib/typeinfo.hh"

namespace ata
{
	namespace sata
	{
		struct HbaCmdList;
		struct HbaRevFis;
		struct FisRegH2D;

		class SataDriver
		{
		protected:
			smp::Lock _lock;
			void *_cfg_addr;
			HbaCmdList *_clb;
			HbaRevFis *_fb;

		public:
			SataDriver() = default;
			void init( const char * lock_name, void *clb, void *fb );

			virtual void test() = 0;
		};
	} // namespace sata

} // namespace ata


//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hal/ata/ata_cmd.hh"
#include "hal/sata/sata_driver.hh"

namespace ata
{
	namespace sata
	{
		struct HbaMemVendorRegLs2k
		{
			dword bistafr;
			dword bistcr;
			dword bistctr;
			dword bistsr;
			dword bistdecr;
			dword oobr;
			dword timer1ms;
			dword gparam1r;
			dword gparam2r;
			dword pparamr;
			dword testr;
			dword versionr;
			dword idr;
		}__attribute__( ( __packed__ ) );

		class SataLs2k : public SataDriver
		{
		private:

		public:
			SataLs2k() : SataDriver() {};
			void init( const char * lock_name, void *clb, void *fb );
		};

		extern SataLs2k k_sata_driver;
	} // namespace sata
	
} // namespace ata

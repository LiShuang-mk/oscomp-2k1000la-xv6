//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hba_param.hh"

namespace ata
{
	namespace sata
	{
		/// @brief refer to AHCI - 3.3 Port Registers 
		struct  HbaPortReg
		{
			dword clb;
			dword clbu;
			dword fb;
			dword fbu;
			dword is;
			dword ie;
			dword cmd;
			dword tfd;
			dword sig;
			dword ssts;
			dword sctl;
			dword serr;
			dword sact;
			dword ci;
			dword sntf;
			dword fbs;
			dword devslp;

			// vendor specific from 70h to 7fh 
			byte vendor_reg[ 0x80 - 0x70 ];
		}__attribute__( ( __packed__ ) );


	} // namespace sata

} // namespace ata

//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

/**
 * 这个文件用于抽象HBA(Host Bus Adapter)
 * 详细可参考 AHCI文档
*/

#pragma once 

#include "hal/ata/ata_types.hh"
#include "hal/sata/hba_param.hh"
#include "hal/sata/hba_port.hh"

namespace ata
{
	namespace sata
	{		
		/// @brief refer to AHCI - 3. HBA Memory Register
		struct HbaMemReg
		{
			// generic host control 

			dword cap;
			dword ghc;
			dword is;
			dword pi;
			dword vs;
			dword ccc_ctl;
			dword ccc_ports;
			dword em_loc;
			dword em_ctl;
			dword cap2;
			dword bohc;

			// reserve from 2Ch to 5Fh
			byte _resv[ 0x60 - 0x2C ];

			// reserve from 60h to 9Fh for NVMHCI 
			byte _resv_nvmhci[ 0xA0 - 0x60 ];

			// vendor specific register 
			// from A0h to FFh
			byte vendor_reg[ 0x100 - 0x0A0 ];

			HbaPortReg ports[ hba_port_num ];
		};

	} // namespace sata
	
} // namespace ata

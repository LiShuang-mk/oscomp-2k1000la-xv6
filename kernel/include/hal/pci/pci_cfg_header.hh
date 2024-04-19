//
// Created by Li shuang ( pseudonym ) on 2024-04-09 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace dev
{
	namespace pci
	{
		struct PciCfgHeader
		{
			uint16 vendor_id;
			uint16 device_id;
			uint16 command;
			uint16 status;
			uint8  revision_id;
			uint32 class_code : 24;
			uint8  cache_line_size;
			uint8  latency_timer;
			uint8  header_type;
			uint8  bist;
			uint32 base_address[ 6 ];
			uint32 cardbus_cis_pointer;
			uint16 subsystem_vendor_id;
			uint16 subsystem_id;
			uint32 expansion_rom_base_address;
			uint8  capabilities_pointer;
			uint32 resv0 : 24;
			uint32 resv1;
			uint8  interrupt_line;
			uint8  interrupt_pin;
			uint8  min_gnt;
			uint8  max_lat;
		}__attribute__( ( __packed__ ) );

	} // namespace pci
}

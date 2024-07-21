//
// Created by Li Shuang ( pseudonym ) on 2024-07-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include <pci/pci_config_header.hh>

namespace loongarch
{
	namespace qemu2k1000
	{
		constexpr ulong pci_type0_config_base_addr = 0xFE0UL << 28;
		constexpr ulong pci_type1_config_base_addr = 0xFE1UL << 28;
		constexpr ulong pci_bus_num_shift = 16;
		constexpr ulong pci_dev_num_shift = 11;
		constexpr ulong pci_fun_num_shift = 8;

		constexpr ulong pci_type0_config_address( uint bus_num, uint dev_num, uint fun_num )
		{
			return pci_type0_config_base_addr |
				( ( ulong ) bus_num << pci_bus_num_shift ) |
				( ( ulong ) dev_num << pci_dev_num_shift ) |
				( ( ulong ) fun_num << pci_fun_num_shift );
		}
		constexpr ulong pci_type1_config_address( uint bus_num, uint dev_num, uint fun_num )
		{
			return pci_type1_config_base_addr |
				( ( ulong ) bus_num << pci_bus_num_shift ) |
				( ( ulong ) dev_num << pci_dev_num_shift ) |
				( ( ulong ) fun_num << pci_fun_num_shift );
		}
		constexpr ulong pci_type0_bar( ulong cfg_addr, int bar_index )
		{
			hsai::PciType0ConfigHeader * cfg = ( hsai::PciType0ConfigHeader * ) cfg_addr;
			ulong * bars = ( ulong * ) cfg->base_address;
			return bars[ bar_index ];
		}


	} // namespace qemu2k1000

} // namespace loongarch

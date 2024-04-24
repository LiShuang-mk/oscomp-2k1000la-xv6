//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace loongarch
{
	namespace qemuls2k
	{
		constexpr uint64 dmwin_mask = 0xFUL << 60;
		enum dmwin : uint64
		{
			win_0 = 0x9UL << 60,
			win_1 = 0x8UL << 60,
		};
		constexpr uint64 virt_to_phy_address( uint64 virt )
		{
			return virt & ~dmwin_mask;
		}

		/// @brief interface address 
		enum InterAddr : uint64
		{
			uart0 = 0x1fe20000UL | dmwin::win_0,
		};

		enum memory : uint64
		{
			mem_start = 0x90000000UL | dmwin::win_0,
			mem_size = CommonSize::_1M << 7, 			// 128M 
			mem_end = mem_start + mem_size
		};

		/// @brief 2k1000la configure registers base address 
		constexpr uint64 config_reg_base = 0x1fe00000UL | dmwin::win_0;

		/// @brief interrupt configure  
		enum ItrCfg : uint64
		{
			itr_low_bit = 0x1420UL + config_reg_base,
			itr_high_bit = 0x1460UL + config_reg_base,
			itr_route_base = 0x1400UL + config_reg_base,

			itr_bit_uart0_s = 0,
			itr_bit_uart0_m = 0x1UL << itr_bit_uart0_s,
			itr_bit_sata_s = 19,
			itr_bit_sata_m = 0x1UL << itr_bit_sata_s,

			itr_route_uart0 = 0x0UL + itr_route_base,
			itr_route_sata = 0x13UL + itr_route_base,

			itr_isr_l = 0x00UL + itr_low_bit,
			itr_isr_h = 0x00UL + itr_high_bit,
			itr_esr_l = 0x04UL + itr_low_bit,
			itr_esr_h = 0x04UL + itr_high_bit,
			itr_enr_l = 0x08UL + itr_low_bit,
			itr_enr_h = 0x08UL + itr_high_bit,
			itr_clr_l = 0x0cUL + itr_low_bit,
			itr_clr_h = 0x0cUL + itr_high_bit,
			itr_pol_l = 0x10UL + itr_low_bit,
			itr_pol_h = 0x10UL + itr_high_bit,
			itr_edg_l = 0x14UL + itr_low_bit,
			itr_edg_h = 0x14UL + itr_high_bit,
			itr_bou_l = 0x18UL + itr_low_bit,
			itr_bou_h = 0x18UL + itr_high_bit,
			itr_aut_l = 0x1cUL + itr_low_bit,
			itr_aut_h = 0x1cUL + itr_high_bit,
		};

		/// @brief route function 
		/// @param x > core number 
		/// @param y > pin number 
		/// @return 
		constexpr inline uchar itr_route_xy( uchar x, uchar y )
		{
			return ( ( 0x1U << x ) << 0 ) | ( ( 0x1U << y ) << 4 );
		}

		constexpr inline void write_itr_cfg( ItrCfg itrReg, uint32 data )
		{
			*( volatile uint32 * ) itrReg = data;
		}
		inline uint32 read_itr_cfg( ItrCfg itrReg )
		{
			return *( ( volatile uint32 * ) itrReg );
		}

		constexpr uint64 pci_type0_base = 0xFE00000000 | dmwin::win_0;
		constexpr uint64 pci_type1_base = 0xFE10000000 | dmwin::win_1;

		enum PciCfg : uint64
		{
			pci_cfg_offseth_s = 24,		// offset[11:8]
			pci_cfg_offsetl_s = 0,		// offset[7:0]
			pci_cfg_busnum_s = 16,
			pci_cfg_devnum_s = 11,
			pci_cfg_funnum_s = 8,

			pci_cfg_offseth_m = 0xFUL << pci_cfg_offseth_s,
			pci_cfg_offsetl_m = 0xFFUL << pci_cfg_offsetl_s,
			pci_cfg_busnum_m = 0xFFUL << pci_cfg_busnum_s,
			pci_cfg_devnum_m = 0x1FUL << pci_cfg_devnum_s,
			pci_cfg_funnum_m = 0x7UL << pci_cfg_funnum_s,
		};

		enum PciDev : uint
		{
			apb_bus = 0x0,
			apb_dev = 0x2,
			apb_fun = 0x0,

			gmac0_bus = 0x0,
			gmac0_dev = 0x3,
			gmac0_fun = 0x0,

			gmac1_bus = 0x0,
			gmac1_dev = 0x3,
			gmac1_fun = 0x1,

			usbotg_bus = 0x0,
			usbotg_dev = 0x4,
			usbotg_fun = 0x0,

			usbehci_bus = 0x0,
			usbehci_dev = 0x4,
			usbehci_fun = 0x1,

			usbohci_bus = 0x0,
			usbohci_dev = 0x4,
			usbohci_fun = 0x2,

			gpu_bus = 0x0,
			gpu_dev = 0x5,
			gpu_fun = 0x0,

			dc_bus = 0x0,
			dc_dev = 0x6,
			dc_fun = 0x0,

			hda_bus = 0x0,
			hda_dev = 0x7,
			hda_fun = 0x0,

			sata_bus = 0x0,
			sata_dev = 0x8,
			sata_fun = 0x0,

			pcie0p0_bus = 0x0,
			pcie0p0_dev = 0x9,
			pcie0p0_fun = 0x0,

			pcie0p1_bus = 0x0,
			pcie0p1_dev = 0xa,
			pcie0p1_fun = 0x0,

			pcie0p2_bus = 0x0,
			pcie0p2_dev = 0xb,
			pcie0p2_fun = 0x0,

			pcie0p3_bus = 0x0,
			pcie0p3_dev = 0xc,
			pcie0p3_fun = 0x0,

			pcie1p0_bus = 0x0,
			pcie1p0_dev = 0xd,
			pcie1p0_fun = 0x0,

			pcie1p1_bus = 0x0,
			pcie1p1_dev = 0xe,
			pcie1p1_fun = 0x0,

			dma_bus = 0x0,
			dma_dev = 0xf,
			dma_fun = 0x0,
		};

#define _build_pci_cfg_base_(name) pci_cfg_##name = pci_type0_base | ( name##_bus << pci_cfg_busnum_s ) | ( name##_dev << pci_cfg_devnum_s ) | ( name##_fun << pci_cfg_funnum_s )

		enum PciCfgDevAddr : uint64
		{
			_build_pci_cfg_base_( apb ),
			_build_pci_cfg_base_( gmac0 ),
			_build_pci_cfg_base_( gmac1 ),
			_build_pci_cfg_base_( usbotg ),
			_build_pci_cfg_base_( usbehci ),
			_build_pci_cfg_base_( usbohci ),
			_build_pci_cfg_base_( gpu ),
			_build_pci_cfg_base_( dc ),
			_build_pci_cfg_base_( hda ),
			_build_pci_cfg_base_( sata ),
			_build_pci_cfg_base_( pcie0p0 ),
			_build_pci_cfg_base_( pcie0p1 ),
			_build_pci_cfg_base_( pcie0p2 ),
			_build_pci_cfg_base_( pcie0p3 ),
			_build_pci_cfg_base_( pcie1p0 ),
			_build_pci_cfg_base_( pcie1p1 ),
			_build_pci_cfg_base_( dma ),
		};

#undef _build_pci_cfg_base_

		constexpr uint64 xbar_win4_cfg_base = 0x1fe02400UL | dmwin::win_1;

		constexpr uint64 iodma_win_base = 0x0UL << 60;
		constexpr uint64 iodma_win_mask = ~( 0x0UL ) << 32;

		enum XbarWin : uint64
		{
			xwin4_base0 = 0x00 + xbar_win4_cfg_base,
			xwin4_base1 = 0x08 + xbar_win4_cfg_base,
			xwin4_base2 = 0x10 + xbar_win4_cfg_base,
			xwin4_base3 = 0x18 + xbar_win4_cfg_base,
			xwin4_base4 = 0x20 + xbar_win4_cfg_base,
			xwin4_base5 = 0x28 + xbar_win4_cfg_base,
			xwin4_base6 = 0x30 + xbar_win4_cfg_base,
			xwin4_base7 = 0x38 + xbar_win4_cfg_base,

			xwin4_mask0 = 0x40 + xbar_win4_cfg_base,
			xwin4_mask1 = 0x48 + xbar_win4_cfg_base,
			xwin4_mask2 = 0x50 + xbar_win4_cfg_base,
			xwin4_mask3 = 0x58 + xbar_win4_cfg_base,
			xwin4_mask4 = 0x60 + xbar_win4_cfg_base,
			xwin4_mask5 = 0x68 + xbar_win4_cfg_base,
			xwin4_mask6 = 0x70 + xbar_win4_cfg_base,
			xwin4_mask7 = 0x78 + xbar_win4_cfg_base,

			xwin4_mmap0 = 0x80 + xbar_win4_cfg_base,
			xwin4_mmap1 = 0x88 + xbar_win4_cfg_base,
			xwin4_mmap2 = 0x90 + xbar_win4_cfg_base,
			xwin4_mmap3 = 0x98 + xbar_win4_cfg_base,
			xwin4_mmap4 = 0xa0 + xbar_win4_cfg_base,
			xwin4_mmap5 = 0xa8 + xbar_win4_cfg_base,
			xwin4_mmap6 = 0xb0 + xbar_win4_cfg_base,
			xwin4_mmap7 = 0xb8 + xbar_win4_cfg_base,
		};

		enum XbarWinMmap : uint64
		{
			xbar_win_num_s = 0,
			xbar_win_cac_s = 5,
			xbar_win_typ_s = 6,
			xbar_win_ena_s = 7,

			xbar_win_num_m = 0x1 << xbar_win_num_s,
			xbar_win_cac_m = 0x1 << xbar_win_cac_s,
			xbar_win_typ_m = 0x1 << xbar_win_typ_s,
			xbar_win_ena_m = 0x1 << xbar_win_ena_s,
		};

		constexpr uint64 second_cpu_win_cfg_base = 0x1fe02000UL | dmwin::win_1;
		constexpr uint64 second_pci_win_cfg_base = 0x1fe02100UL | dmwin::win_1;
#define _build_cpu_win_( num ) \
	cpu_win##num##_base = second_cpu_win_cfg_base + 0x00UL + num * 8, \
	cpu_win##num##_mask = second_cpu_win_cfg_base + 0x40UL + num * 8, \
	cpu_win##num##_mmap = second_cpu_win_cfg_base + 0x80UL + num * 8,
#define _build_pci_win_( num ) \
	pci_win##num##_base = second_pci_win_cfg_base + 0x00UL + num * 8, \
	pci_win##num##_mask = second_pci_win_cfg_base + 0x40UL + num * 8, \
	pci_win##num##_mmap = second_pci_win_cfg_base + 0x80UL + num * 8, 
		enum SecondWin : uint64
		{
			_build_cpu_win_( 0 )
			_build_cpu_win_( 1 )
			_build_cpu_win_( 2 )
			_build_cpu_win_( 3 )
			_build_cpu_win_( 4 )
			_build_cpu_win_( 5 )
			_build_cpu_win_( 6 )
			_build_cpu_win_( 7 )

			_build_pci_win_( 0 )
			_build_pci_win_( 1 )
			_build_pci_win_( 2 )
			_build_pci_win_( 3 )
			_build_pci_win_( 4 )
			_build_pci_win_( 5 )
			_build_pci_win_( 6 )
			_build_pci_win_( 7 )
		};
#undef _build_cpu_win_
#undef _build_pci_win_

		constexpr uint64 sec_win_route_ddr = 0;
		constexpr uint64 sec_win_route_spi = 2;
		enum SecondWinMmap : uint64
		{
			sec_win_rt_s = 0,	// route 
			sec_win_fi_s = 4, 	// allow fetch instructions 
			sec_win_br_s = 5,	// allow block read 
			sec_win_en_s = 7, 	// enable 

			sec_win_rt_m = 0x7UL << sec_win_rt_s,
			sec_win_fi_m = 0x1UL << sec_win_fi_s,
			sec_win_br_m = 0x1UL << sec_win_br_s,
			sec_win_en_m = 0x1UL << sec_win_en_s,
		};


	} // namespace qemuls2k

} // namespace name

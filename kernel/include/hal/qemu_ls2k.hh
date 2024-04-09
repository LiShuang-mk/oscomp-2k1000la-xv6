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
		enum ls2kPgEnum : uint64
		{
			pg_flags_mask = 0xE0000000000001FFUL,
		};

		enum dmwin : uint64
		{
			win_0 = 0x9UL << 60,
			win_1 = 0x8UL << 60,
		};

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

			itr_route_uart0 = 0x0UL + itr_route_base,

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
	} // namespace qemuls2k

} // namespace name

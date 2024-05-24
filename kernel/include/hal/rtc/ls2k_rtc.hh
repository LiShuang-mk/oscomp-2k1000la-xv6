//
// Created by Li Shuang ( pseudonym ) on 2024-05-24 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hal/qemu_ls2k.hh"

namespace loongarch
{
	namespace qemuls2k
	{
		constexpr uint64 rtc_base_addr = 0x1fe27800 | loongarch::qemuls2k::win_1;

#define _build_rtcreg_( name, offset ) rtc_##name = offset + rtc_base_addr,
		enum RtcRegAddr : uint64
		{
			_build_rtcreg_( toytrim, 0x20 )
			_build_rtcreg_( toywrite0, 0x24 )
			_build_rtcreg_( toywrite1, 0x28 )
			_build_rtcreg_( toyread0, 0x2c )
			_build_rtcreg_( toyread1, 0x30 )
			_build_rtcreg_( toymatch0, 0x34 )
			_build_rtcreg_( toymatch1, 0x38 )
			_build_rtcreg_( toymatch2, 0x3c )
			_build_rtcreg_( rtcctrl, 0x40 )
			_build_rtcreg_( rtctrim, 0x60 )
			_build_rtcreg_( rtcwrite0, 0x64 )
			_build_rtcreg_( rtcread0, 0x68 )
			_build_rtcreg_( rtcmatch0, 0x6c )
			_build_rtcreg_( rtcmatch1, 0x70 )
			_build_rtcreg_( rtcmatch2, 0x74 )
		};
#undef _build_rtcreg_

#define _build_rtcctrl_bit_( name, shift, mask ) \
	rtcctrl_##name##_s = shift, \
	rtcctrl_##name##_m = mask << rtcctrl_##name##_s,
		enum RtcRegRtcctrl : uint32
		{
			_build_rtcctrl_bit_( ers, 23, 0x1 )
			_build_rtcctrl_bit_( rts, 20, 0x1 )
			_build_rtcctrl_bit_( rm2, 19, 0x1 )
			_build_rtcctrl_bit_( rm1, 18, 0x1 )
			_build_rtcctrl_bit_( rm0, 17, 0x1 )
			_build_rtcctrl_bit_( rs, 16, 0x1 )
			_build_rtcctrl_bit_( ren, 13, 0x1 )
			_build_rtcctrl_bit_( ten, 11, 0x1 )
			_build_rtcctrl_bit_( eo, 8, 0x1 )
			_build_rtcctrl_bit_( 32s, 5, 0x1 )
			_build_rtcctrl_bit_( tm2, 3, 0x1 )
			_build_rtcctrl_bit_( tm1, 2, 0x1 )
			_build_rtcctrl_bit_( tm0, 1, 0x1 )
			_build_rtcctrl_bit_( ts, 0, 0x1 )
		};
#undef _build_rtcctrl_bit_
		
	} // namespace qemu_ls2k

} // namespace loongarch

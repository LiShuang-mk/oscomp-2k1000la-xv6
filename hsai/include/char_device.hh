//
// Created by Li Shuang ( pseudonym ) on 2024-07-15 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "virtual_device.hh"

namespace hsai
{
	class CharDevice : public VirtualDevice
	{
	public:
		CharDevice() = default;
		virtual DeviceType type() override { return DeviceType::dev_char; }
		virtual bool support_stream() = 0;
		virtual int get_char_sync( u8 *c ) = 0;
		virtual int get_char( u8 *c ) = 0;
		virtual int put_char_sync( u8 c ) = 0;
		virtual int put_char( u8 c ) = 0;
		virtual int handle_intr() = 0;
	};

} // namespace hsai

//
// Created by Li Shuang ( pseudonym ) on 2024-07-15 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

namespace hsai
{
	class CharDevice
	{
	public:
		virtual uint8 get_char_sync() = 0;
		virtual uint8 get_char() = 0;
		virtual int put_char_sync( u8 c ) = 0;
		virtual int put_char( u8 c ) = 0;
	};

} // namespace hsai

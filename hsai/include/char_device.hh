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
		virtual uint8 put_char_sync() = 0;
		virtual uint8 put_char() = 0;
	};

} // namespace hsai

//
// Created by Li Shuang ( pseudonym ) on 2024-07-25 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "char_device.hh"

namespace hsai
{
	class StreamDevice : public CharDevice
	{
	protected:
		CharDevice * _stream = nullptr;

	public:
		StreamDevice() = default;
		virtual ~StreamDevice() = default;
		virtual bool support_stream() override { return true; }
		virtual int get_char_sync( u8 *c ) override;
		virtual int get_char( u8 *c ) override;
		virtual int put_char_sync( u8 c ) override;
		virtual int put_char( u8 c ) override;
		virtual int handle_intr() override;

		virtual long write( void * src, long n_bytes ) = 0;
		virtual long read( void * dst, long n_bytes ) = 0;

	public:
		int redirect_stream( CharDevice * dev );
	};
} // namespace hsai

//
// Created by Li Shuang ( pseudonym ) on 2024-07-25 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "stream_device.hh"
#include "hsai_log.hh"

namespace hsai
{
	int StreamDevice::redirect_stream( CharDevice * dev )
	{
		if ( dev->type() != dev_char )
		{
			hsai_warn( "try to bind stream with a device that's not char-dev" );
			return -1;
		}
		_stream = dev;
		return 0;
	}

	int StreamDevice::get_char_sync( u8 *c )
	{
		if ( _stream != nullptr )
			return _stream->get_char_sync( c );
		return 0;
	}
	int StreamDevice::get_char( u8 *c )
	{
		if ( _stream != nullptr )
			return _stream->get_char( c );
		return 0;
	}
	int StreamDevice::put_char_sync( u8 c )
	{
		if ( _stream != nullptr )
			return _stream->put_char_sync( c );
		return 0;
	}
	int StreamDevice::put_char( u8 c )
	{
		if ( _stream != nullptr )
			return _stream->put_char( c );
		return 0;
	}
	int StreamDevice::handle_intr()
	{
		if ( _stream != nullptr )
			return _stream->handle_intr();
		return 0;
	}

} // namespace hsai

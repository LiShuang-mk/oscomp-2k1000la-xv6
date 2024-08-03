#include "fs/file/device.hh"
#include "hsai_global.hh"
#include "device_manager.hh"
#include "stream_device.hh"

#include <termios.h>

namespace fs
{
	device_file::device_file( FileAttrs attrs, uint dev )
		: file( attrs )
	{
		hsai::StreamDevice * sdev = ( hsai::StreamDevice * ) hsai::k_devm.get_device( dev );
		if ( sdev == nullptr )
		{
			log_error( "file write: null device for device number %d", dev );
			return;
		}
		if ( sdev->type() != hsai::dev_char )
		{
			log_error( "file write: device %d is not a char-dev", dev );
			return;
		}
		if ( !sdev->support_stream() )
		{
			log_error( "file write: device %d is not a stream-dev", dev );
			return;
		}
		_dev = sdev;
		dup();
	}

	long device_file::read( uint64 buf, size_t len, long off, bool upgrade )
	{
		int ret;

		if ( _attrs.u_read != 1 )
		{
			log_error( "device_file:: not allowed to read! " );
			return -1;
		}

		ret = _dev->read( ( void * ) buf, len );

		/// @note 对于流式设备而言，没有文件偏移的概念
		// if ( ret >= 0 && upgrade )  // 这里 没有写sdev从指定位置读取的函数
		// 	off += ret;
		return ret;

	}

	long device_file::write( uint64 buf, size_t len, long off, bool upgrade )
	{
		int ret;

		ret = _dev->write( ( void * ) buf, len );
		return ret;
	}

	bool device_file::read_ready()
	{
		return _dev->read_ready();
	}

	bool device_file::write_ready()
	{
		return _dev->write_ready();
	}

	int device_file::tcgetattr( termios * ts )
	{
		// ts->c_ispeed = ts->c_ospeed = B115200;

		return 0;
	}
}
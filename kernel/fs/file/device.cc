#include "fs/file/device.hh"
#include "hsai_global.hh"
#include "device_manager.hh"
#include "stream_device.hh"

#include <termios.h>

namespace fs
{
    int device_file::read( uint64 buf, size_t len, int off, bool upgrade ) 
    {
        int ret;

		if( _attrs.u_read != 1 )
		{
			log_error( "device_file:: not allowed to read! "); 
            return -1;
		}
		
        hsai::StreamDevice * sdev = ( hsai::StreamDevice * ) hsai::k_devm.get_device( dev );
		if ( sdev == nullptr )
		{
			log_error( "file write: null device for device number %d", dev );
			return -1;
		}
		if ( sdev->type() != hsai::dev_char )
		{
			log_warn( "file write: device %d is not a char-dev", dev );
			return -2;
		}
		if ( !sdev->support_stream() )
				{
			log_warn( "file write: device %d is not a stream-dev", dev );
			return -3;
		}

		ret = sdev->read( ( void * )buf, len );
		if( ret >= 0 && upgrade )  // 这里 没有写sdev从指定位置读取的函数
			off += ret;
		return ret;

    }

    int device_file::write( uint64 buf, size_t len )
    {
		int ret;

		if( _attrs.u_write != 1 )
		{	
			log_error( "device_file:: not allowed to write! "); 
			return -1;
		}
		hsai::StreamDevice * sdev = ( hsai::StreamDevice * ) hsai::k_devm.get_device( dev );
		if ( sdev == nullptr )
		{
			log_error( "file write: null device for device number %d", dev );
			return -1;
		}
		if ( sdev->type() != hsai::dev_char )
		{
			log_warn( "file write: device %d is not a char-dev", dev );
			return -2;
		}
		if ( !sdev->support_stream() )
		{
			log_warn( "file write: device %d is not a stream-dev", dev );
			return -3;
		}

		ret = sdev->write( ( void * ) buf, len );
		return ret;
	}


	int device_file::tcgetattr( termios * ts )
	{
		// ts->c_ispeed = ts->c_ospeed = B115200;
		return 0;
	}
}
#include "fs/file/device.hh"
#include "hsai_global.hh"
#include "device_manager.hh"
#include "stream_device.hh"
#include "fs/dentry.hh"

#include <termios.h>

namespace fs
{

	long device_file::read( uint64 buf, size_t len, long off, bool upgrade )
	{
		int ret;

		if ( _attrs.u_read != 1 )
		{
			log_error( "device_file:: not allowed to read! " );
			return -1;
		}

		Inode *node = _dentry->getNode();
		
		if ( node == nullptr )
		{
			log_error( "device_file:: null inode for dentry %s", _dentry->rName().c_str() );
			return -1;
		}

		ret = node->nodeRead( buf, off, len );

		/// @note 对于流式设备而言，没有文件偏移的概念
		// if ( ret >= 0 && upgrade )  // 这里 没有写sdev从指定位置读取的函数
		// 	off += ret;
		return ret;

	}

	long device_file::write( uint64 buf, size_t len, long off, bool upgrade )
	{
		int ret;

		if( _attrs.u_write != 1 )
		{
			log_error( "device_file:: not allowed to write! " );
			return -1;
		}

		Inode 	*node = _dentry->getNode();
		
		if( node == nullptr )
		{
			log_error( "device_file:: null inode for dentry %s", _dentry->rName().c_str() );
			return -1;
		}

		ret = node->nodeWrite( buf, off, len );
		//ret = _dev->write( ( void * ) buf, len );
		return ret;
	}

	bool device_file::read_ready()
	{
		if( _dev == nullptr )
		{
			dev_t dev = _dentry->getNode()->rDev();
			_dev = ( hsai::StreamDevice * ) hsai::k_devm.get_device( dev );
			
			if( _dev == nullptr )
			{
				log_error( "device_file::read_ready: null device for device number %d", dev );
				return false;
			}

			if( _dev->type() != hsai::dev_char )
			{
				log_error( "device_file::read_ready: device %d is not a char-dev", dev );
				return false;
			}

			if( !_dev->support_stream() )
			{
				log_error( "device_file::read_ready: device %d is not a stream-dev", dev );
				return false;
			}
		}

		return _dev->read_ready();
	}

	bool device_file::write_ready()
	{
		if( _dev == nullptr )
		{
			dev_t dev = _dentry->getNode()->rDev();
			_dev = ( hsai::StreamDevice * ) hsai::k_devm.get_device( dev );
			
			if( _dev == nullptr )
			{
				log_error( "device_file::read_ready: null device for device number %d", dev );
				return false;
			}

			if( _dev->type() != hsai::dev_char )
			{
				log_error( "device_file::read_ready: device %d is not a char-dev", dev );
				return false;
			}

			if( !_dev->support_stream() )
			{
				log_error( "device_file::read_ready: device %d is not a stream-dev", dev );
				return false;
			}
		}
		
		return _dev->write_ready();
	}

	int device_file::tcgetattr( termios * ts )
	{
		// ts->c_ispeed = ts->c_ospeed = B115200;

		return 0;
	}
}
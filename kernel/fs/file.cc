//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/file/file.hh"
#include "fs/dentry.hh"
#include "fs/inode.hh"

#include <hsai_global.hh>
#include <device_manager.hh>
#include <stream_device.hh>

namespace fs
{
	int File::write( uint64 buf, size_t len )
	{
		int RC = -1;
		if ( !ops.fields.w )
		{
			log_error( "File is not allowed to write!\n" );
			return RC;
		}
		log_info( "write %d bytes...\n", len );

		switch ( type )
		{
			case FileTypes::FT_STDOUT:
			case FileTypes::FT_STDERR:
				printf( "%s", ( char * ) buf );
				RC = len;
				break;
			case FileTypes::FT_PIPE:
			{
				if ( static_cast< size_t >( data.get_Pipe()->write( buf, len ) ) == len )
				{
					RC = len;
				}
				break;
			}
			case FileTypes::FT_ENTRY:
			{
				if ( data.get_Entry()->getNode()->nodeWrite( buf, data.get_off(), len ) == len )
				{
					data.get_off() += len;
					RC = len;
				}
				break;
			}
			case FileTypes::FT_DEVICE:
			{
				log_panic( "file type device not implement" );
				// hsai::StreamDevice * sdev = ( hsai::StreamDevice * ) hsai::k_devm.get_device( major );
				// if ( sdev == nullptr )
				// {
				// 	log_error( "file write: null device for device number %d", major );
				// 	return -1;
				// }
				// if ( sdev->type() != hsai::dev_char )
				// {
				// 	log_warn( "file write: device %d is not a char-dev", major );
				// 	return -2;
				// }
				// if ( !sdev->support_stream() )
				// {
				// 	log_warn( "file write: device %d is not a stream-dev", major );
				// 	return -3;
				// }

				// ret = sdev->write( ( void * ) addr, n );

			} break;
			default:
				log_panic( "file::write(),Unknown File Type!\n" );
				break;
		}

		return RC;
	}

	int File::read( uint64 buf, size_t len, int off_, bool update )
	{
		int RC = -1;
		if ( !ops.fields.r )
		{
			log_error( "File is not allowed to read!\n" );
			return RC;
		}
		switch ( data.get_Type() )
		{
			case FileTypes::FT_STDIN:
				log_error( "stdin is not allowed to read!\n" );
				break;
			case FileTypes::FT_DEVICE:
				log_error( "Device is not allowed to read!\n" );
				break;
			case FileTypes::FT_PIPE:
			{
				if ( auto read_len = data.get_Pipe()->read( buf, len ) )
				{
					RC = read_len;
				}
				break;
			}
			case FileTypes::FT_ENTRY:
			{
				if ( off_ < 0 )
					off_ = data.get_off();
				if ( auto read_len = data.get_Entry()->getNode()->nodeRead( buf, off_, len ) )
				{
					if ( update )
						data.get_off() += read_len;
					RC = read_len;
				}
				break;
			}
			default:
				log_panic( "file::read(),Unknown File Type!\n" );
				break;
		}
		return RC;
	}

// ================ xv6 file ================

	int xv6_file::write( uint64 addr, int n )
	{
		int ret;
		switch ( type )
		{

			case FD_PIPE:
			{
				ret = pipe->write( addr, n );
			} break;

			case FD_DEVICE:
			{
				hsai::StreamDevice * sdev = ( hsai::StreamDevice * ) hsai::k_devm.get_device( major );
				if ( sdev == nullptr )
				{
					log_error( "file write: null device for device number %d", major );
					return -1;
				}
				if ( sdev->type() != hsai::dev_char )
				{
					log_warn( "file write: device %d is not a char-dev", major );
					return -2;
				}
				if ( !sdev->support_stream() )
				{
					log_warn( "file write: device %d is not a stream-dev", major );
					return -3;
				}

				ret = sdev->write( ( void * ) addr, n );

			} break;

			case FD_INODE:
			{
				// log_panic( "file inode write not implement" );
				ret = n;
			} break;

			default:
				break;
		}

		return ret;
	}

	int xv6_file::read( uint64 addr, int n )
	{
		int ret;
		switch ( type )
		{

			case FD_PIPE:
			{
				ret = pipe->read( addr, n );
			} break;

			case FD_DEVICE:
			{
				log_panic( "file device read not implement" );
			} break;

			case FD_INODE:
			{
				uint64 len = dentry->getNode()->nodeRead( addr, off, n );
				off += len;
				ret = ( int ) len;
			} break;

			default:
				break;
		}

		return ret;
	}

// ================ xv6 file pool ================	

	file_pool k_file_table;

	void file_pool::init()
	{
		_lock.init( "file pool" );
		for ( auto &f : _files )
		{	// refcnt 的初始化在构造参数中
			// f.ref = 0;
			f.type = fs::FileTypes::FT_NONE;
		}
	}

	File * file_pool::alloc_file()
	{
		_lock.acquire();
		for ( auto &f : _files )
		{
			if ( f.refcnt == 0 && f.type == FileTypes::FT_NONE )
			{
				f.refcnt = 1;
				_lock.release();
				return &f;
			}
		}
		_lock.release();
		return nullptr;
	}

	void file_pool::free_file( File * f )
	{
		_lock.acquire();
		if ( f->refcnt <= 0 )
		{
			log_error( "[file pool] free no-ref file" );
			_lock.release();
			return;
		}
		--f->refcnt;
		if ( f->refcnt == 0 )
		{
			if ( f->type == FileTypes::FT_PIPE )
				//f->pipe->close( f->writable );
				f->data.get_Pipe()->close( f->ops.fields.w );
			f->type = FileTypes::FT_NONE;
			f->flags = 0; 
			f->ops = FileOps( 0 );
			//Placement new
			new ( &f->data ) File::Data( FileTypes::FT_NONE );
		}
		_lock.release();
	}

	void file_pool::dup( File *f )
	{
		_lock.acquire();
		assert( f->refcnt >= 1, "file: try to dup no reference file." );
		f->refcnt++;
		_lock.release();
	}

	File * file_pool::find_file( eastl::string path )
	{
		_lock.acquire();
		for ( auto &f : _files )
		{
			dentry *den = f.data.get_Entry();
			if ( den && den->rName() == path )
			{
				_lock.release();
				return &f;
			}
		}
		_lock.release();
		return nullptr;
	}

	int file_pool::unlink( eastl::string path )
	{
		_lock.acquire();
		_unlink_list.push_back( path );
		_lock.release();
		return 0;
	}
	
} // namespace fs

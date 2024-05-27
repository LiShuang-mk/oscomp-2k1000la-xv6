//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/device.hh"
#include "fs/file.hh"
#include "fs/dentry.hh"
#include "fs/inode.hh"

namespace fs
{
	int File::write( void *buf, size_t len )
	{
		int RC = -1;
		if ( !ops.fields.w )
		{
			log_error( "File is not allowed to write!\n" );
			return RC;
		}
		log_info( "write %d bytes...\n", len );

		switch ( data.get_Type() )
		{
			case FileTypes::FT_STDOUT:
			case FileTypes::FT_STDERR:
				printf( "%s", ( char * ) buf );
				RC = len;
				break;
			case FileTypes::FT_ENTRY:
			{
				uint64* buf_uint64 = static_cast< uint64* >( buf );
				uint64 buffer = *buf_uint64;
				if ( data.get_Entry()->getNode()->nodeWrite( buffer, data.get_off(), len ) == len )
				{
					data.get_off() += len;
					RC = len;
				}
				break;
			}
			default:
				log_panic( "file::write(),Unknown File Type!\n" );
				break;
		}

		return RC;
	}

	int File::read( void *buf, size_t len, int off_, bool update )
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
			case FileTypes::FT_ENTRY:
			{
				if ( off_ < 0 )
					off_ = data.get_off();
				uint64* buf_uint64 = static_cast< uint64* >( buf );
				uint64 buffer = *buf_uint64;
				if ( auto read_len = data.get_Entry()->getNode()->nodeRead( buffer, off_, len ) )
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
				log_panic( "file pipe write not implement" );
			} break;

			case FD_DEVICE:
			{
				if ( major < 0 || major >( short )dev::max_major_dev_num )
				{
					log_error( "file write: invalid device major number" );
					return -1;
				}
				ret = dev::k_dm.write_device( major, 1, addr, n );

			} break;

			case FD_INODE:
			{
				log_panic( "file inode write not implement" );
			} break;

			default:
				break;
		}

		return ret == n ? n : -1;
	}

	int xv6_file::read( uint64 addr, int n )
	{
		int ret;
		switch ( type )
		{

			case FD_PIPE:
			{
				log_panic( "file pipe read not implement" );
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

	xv6_file_pool k_file_table;

	void xv6_file_pool::init()
	{
		_lock.init( "file pool" );
		for ( auto &f : _files )
		{
			f.ref = 0;
			f.type = xv6_file::FD_NONE;
		}
	}

	xv6_file * xv6_file_pool::alloc_file()
	{
		_lock.acquire();
		for ( auto &f : _files )
		{
			if ( f.ref == 0 && f.type == xv6_file::FD_NONE )
			{
				f.ref = 1;
				_lock.release();
				return &f;
			}
		}
		_lock.release();
		return nullptr;
	}

	void xv6_file_pool::free_file( xv6_file * f )
	{
		_lock.acquire();
		if ( f->ref <= 0 )
		{
			log_error( "[file pool] free no-ref file" );
			_lock.release();
			return;
		}
		--f->ref;
		if ( f->ref == 0 )
		{
			f->readable = f->writable = 0;
			f->dentry = nullptr;
			f->major = 0;
			f->off = 0;
			f->type = xv6_file::FD_NONE;
		}
		_lock.release();
	}

	void xv6_file_pool::dup( xv6_file *f )
	{
		_lock.acquire();
		assert( f->ref >= 1, "file: try to dup no reference file." );
		f->ref++;
		_lock.release();
	}

} // namespace fs

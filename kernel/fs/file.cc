//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

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
				if ( data.get_Entry()->getNode()->nodWrite( buffer, data.get_off(), len ) == len )
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
} // namespace fs

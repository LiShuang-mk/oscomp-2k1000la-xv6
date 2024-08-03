#include "fs/file/normal.hh"

namespace fs
{
	long normal_file::read( uint64 buf, size_t len, long off, bool upgrade )
	{
		long ret;
		if ( _attrs.u_read != 1 )
		{
			log_error( "normal_file:: not allowed to read! " );
			return -1;
		}
		Inode *node = _den->getNode();
		if ( node == nullptr )
		{
			log_error( "normal_file:: null inode for dentry %s", _den->rName().c_str() );
			return -1;
		}
		if ( off < 0 )
			off = _file_ptr;
		ret = node->nodeRead( buf, off, len );
		if ( ret >= 0 && upgrade )
			_file_ptr += ret;
		return ret;
	}

	long normal_file::write( uint64 buf, size_t len, long off, bool upgrade )
	{
		long ret;
		if ( _attrs.u_write != 1 )
		{
			log_error( "normal_file:: not allowed to write! " );
			return -1;
		}
		Inode *node = _den->getNode();
		if ( node == nullptr )
		{
			log_error( "normal_file:: null inode for dentry %s", _den->rName().c_str() );
			return -1;
		}
		if ( off < 0 )
			off = _file_ptr;
		ret = node->nodeWrite( buf, off, len );
		if ( ret >= 0 && upgrade )
			_file_ptr += ret;

		return ret;
	}

	bool normal_file::read_ready()
	{
		if ( _attrs.filetype == FileTypes::FT_DIRECT )
			return false;
		if ( _attrs.filetype == FileTypes::FT_NONE )
			return true;
		log_warn( "normal file is not a directory or regular file." );
		return false;
	}

	bool normal_file::write_ready()
	{
		if ( _attrs.filetype == FileTypes::FT_DIRECT )
			return false;
		if ( _attrs.filetype == FileTypes::FT_NONE )
			return true;
		log_warn( "normal file is not a directory or regular file." );
		return false;
	}
}
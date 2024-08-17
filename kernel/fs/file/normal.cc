#include "fs/file/normal.hh"

#include "mm/userspace_stream.hh"
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
			log_error( "normal_file:: null inode for dentry %s",
					   _den->rName().c_str() );
			return -1;
		}
		if ( off < 0 ) off = _file_ptr;
		ret = node->nodeRead( buf, off, len );
		if ( ret >= 0 && upgrade ) _file_ptr += ret;
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
			log_error( "normal_file:: null inode for dentry %s",
					   _den->rName().c_str() );
			return -1;
		}
		if ( off < 0 ) off = _file_ptr;
		ret = node->nodeWrite( buf, off, len );
		if ( ret >= 0 && upgrade ) _file_ptr += ret;

		return ret;
	}

	bool normal_file::read_ready()
	{
		if ( _attrs.filetype == FileTypes::FT_DIRECT ) return false;
		if ( _attrs.filetype == FileTypes::FT_NONE ) return true;
		log_warn( "normal file is not a directory or regular file." );
		return false;
	}

	bool normal_file::write_ready()
	{
		if ( _attrs.filetype == FileTypes::FT_DIRECT ) return false;
		if ( _attrs.filetype == FileTypes::FT_NONE ) return true;
		log_warn( "normal file is not a directory or regular file." );
		return false;
	}

	size_t normal_file::read_sub_dir( ubuf &dst )
	{
		Inode *ind	 = _den->getNode();
		size_t rlen	 = ind->readSubDir( dst, _file_ptr );
		_file_ptr	+= rlen;
		return rlen;
	}

	off_t normal_file::lseek( off_t offset, int whence )
	{
		off_t size = static_cast<off_t>( this->_stat.size );
		[[maybe_unused]]off_t new_off;
		switch ( whence )
		{
		case SEEK_SET:
			if( offset < 0 || offset > size ) return -EINVAL;
			_file_ptr = offset;
			break;
		case SEEK_CUR:
			new_off = _file_ptr + offset;
			if( new_off < 0 || new_off > size ) return -EINVAL;
			_file_ptr = new_off;
			break;
		case SEEK_END:
			new_off = this->_stat.size + offset;	
			if( new_off < 0 || new_off > size ) return -EINVAL;
			_file_ptr = new_off;
			break;
		default:
			log_error( "normal_file::lseek: invalid whence %d", whence );
			return -EINVAL;
		}
		return _file_ptr;
	}

	void normal_file::setAppend()
	{
		_file_ptr = this->_stat.size;
	}
} // namespace fs

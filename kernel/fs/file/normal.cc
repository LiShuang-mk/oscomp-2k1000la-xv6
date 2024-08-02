#include "fs/file/normal.hh"

namespace fs
{
    int normal_file::read( uint64 buf, size_t len, int off, bool upgrade )
    {
        int ret;
        if( _attrs.u_read != 1 )
        {
            log_error( "normal_file:: not allowed to read! ");
            return -1;
        }
        Inode *node = _den->getNode();
        if( node == nullptr )
        {
            log_error( "normal_file:: null inode for dentry %s", _den->rName().c_str() );
            return -1;
        }
        ret = node->nodeRead( buf, off, len );
        if( ret >= 0 && upgrade )
            _off += ret;
        return ret;
    }

    int normal_file::write( uint64 buf, size_t len )
    {
        int ret;
        if( _attrs.u_write != 1 )
        {
            log_error( "normal_file:: not allowed to write! ");
            return -1;
        }
        Inode *node = _den->getNode();
        if( node == nullptr )
        {
            log_error( "normal_file:: null inode for dentry %s", _den->rName().c_str() );
            return -1;
        }
        ret = node->nodeWrite( buf, 0, len );

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
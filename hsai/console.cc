//
// Created by Li Shuang ( pseudonym ) on 2024-07-25 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "console.hh"
#include "hsai_log.hh"

namespace hsai
{
// >>>> Console - STDIN

	long ConsoleStdin::write( void *, long )
	{
		hsai_warn( "try to write stdin device" );
		return -1;
	}

	long ConsoleStdin::read( void * dst, long nbytes )
	{
		if ( _stream == nullptr )
		{
			hsai_warn( "stream not be bound" );
			return 0;
		}
		u8 * ptr = ( u8 * ) dst;
		for ( long i = 0; i < nbytes; i++, ptr++ )
			if ( _stream->get_char( ptr ) < 0 )
				return i;
		return nbytes;
	}

// <<<< Console - STDIN

// >>>> Console - STDOUT

	long ConsoleStdout::write( void * src, long nbytes )
	{
		if ( _stream == nullptr )
		{
			hsai_warn( "stream not be bound" );
			return 0;
		}
		u8 * ptr = ( u8 * ) src;
		for ( long i = 0; i < nbytes; i++, ptr++ )
			if ( _stream->put_char( *ptr ) < 0 )
				return i;
		return nbytes;
	}

	long ConsoleStdout::read( void *, long )
	{
		hsai_warn( "try to read stdout device" );
		return -1;
	}

// <<<< Console - STDOUT

// >>>> Console - STDERR

	long ConsoleStderr::write( void * src, long nbytes )
	{
		if ( _stream == nullptr )
		{
			hsai_warn( "stream not be bound" );
			return 0;
		}
		u8 * ptr = ( u8 * ) src;
		for ( long i = 0; i < nbytes; i++, ptr++ )
			if ( _stream->put_char_sync( *ptr ) < 0 )
				return i;
		return nbytes;
	}

	long ConsoleStderr::read( void *, long )
	{
		hsai_warn( "try to read stdout device" );
		return -1;
	}

// <<<< Console - STDERR

} // namespace hsai

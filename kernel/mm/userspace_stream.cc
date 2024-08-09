//
// Created by Li shuang ( pseudonym ) on 2024-08-07
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "mm/userspace_stream.hh"

#include "klib/klib.hh"

using namespace mm;

UserspaceStream::UserspaceStream( void *u_addr_start, u64 u_size, PT *u_pt )
	: _start_addr( (ulong) u_addr_start )
	, _end_addr( _start_addr + u_size )
	, _pt( u_pt )
{}

using RC = UserspaceStream::RetCode;

RC UserspaceStream::open()
{
	_ptr = _start_addr;
	_update_cache( (void *) _ptr );

	return rc_ok;
}

RC UserspaceStream::close()
{
	_ptr = 0;
	return rc_ok;
}

UserspaceStream &UserspaceStream::operator<<( UsRangeDesc &rd )
{
	if ( _ptr == 0 )
	{
		_errno = rc_not_open;
		return *this;
	}

	auto [buf, len] = rd;

	len = ( _ptr + len > _end_addr ) ? ( _end_addr - _ptr ) : len;
	while ( len > 0 )
	{
		if ( _cache_ptr + len <= _cache_end )
		{
			memcpy( (void *) _cache_ptr, (void *) buf, len );
			_cache_ptr += len;
			_ptr	   += len;
			_errno		= rc_ok;

			return *this;
		}
		else
		{
			u64 l = _cache_end - _cache_ptr;
			memcpy( (void *) _cache_ptr, (void *) buf, l );
			_cache_ptr += l;
			_ptr	   += l;
			buf		   += l;
			len		   -= l;
			_update_cache( (void *) _ptr );
		}
	}
	_errno = rc_ok;

	return *this;
}

UserspaceStream &UserspaceStream::operator>>( UsRangeDesc &rd )
{
	if ( _ptr == 0 )
	{
		_errno = rc_not_open;
		return *this;
	}

	auto [buf, len] = rd;

	len = ( _ptr + len > _end_addr ) ? ( _end_addr - _ptr ) : len;
	while ( len > 0 )
	{
		if ( _cache_ptr + len <= _cache_end )
		{
			memcpy( (void *) buf, (void *) _cache_ptr, len );
			_cache_ptr += len;
			_ptr	   += len;
			return *this;
		}
		else
		{
			u64 l = _cache_end - _cache_ptr;
			memcpy( (void *) buf, (void *) _cache_ptr, len );
			_cache_ptr += l;
			_ptr	   += l;
			buf		   += l;
			len		   -= l;
			_update_cache( (void *) _ptr );
		}
	}

	return *this;
}



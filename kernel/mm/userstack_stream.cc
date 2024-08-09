//
// Created by Li shuang ( pseudonym ) on 2024-08-07
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "mm/userstack_stream.hh"

using namespace mm;


UserstackStream::UserstackStream( void *u_addr_start, u64 u_size, PT *u_pt )
	: _stack_base( (ulong) u_addr_start )
	, _stack_end( _stack_base + u_size )
	, _pt( u_pt )
{}

using RC = UserstackStream::RetCode;

RC UserstackStream::open()
{
	_sp = _stack_end;
	if ( _update_cache( (void *) ( _sp ) ) < 0 ) return rc_fail;

	return rc_ok;
}

RC UserstackStream::close()
{
	_sp = 0;
	return rc_ok;
}

UserstackStream &UserstackStream::operator<<( UsRangeDesc &rd )
{
	if ( _sp == 0 )
	{
		_errno = rc_not_open;
		return *this;
	}

	auto [buf, len] = rd;

	if ( _sp - len < _stack_base )
	{
		_errno = rc_no_mem;
		return *this;
	}

	buf += len;

	while ( len > 0 )
	{
		if ( _cache_ptr - len >= _cache_begin )
		{
			buf		   -= len;
			_cache_ptr -= len;
			_sp		   -= len;
			memcpy( (void *) ( _cache_ptr ), (void *) buf, len );
			_errno = rc_ok;

			return *this;
		}
		else
		{
			u64 l		= _cache_ptr - _cache_begin;
			_cache_ptr	= _cache_begin;
			buf		   -= l;
			len		   -= l;
			_sp		   -= l;
			memcpy( (void *) _cache_ptr, (void *) buf, l );
			_update_cache( (void *) ( _sp ) );
		}
	}
	_errno = rc_ok;

	return *this;
}

UserstackStream &UserstackStream::operator>>( UsRangeDesc &rd )
{
	if ( _sp == 0 )
	{
		_errno = rc_not_open;
		return *this;
	}

	auto [buf, len] = rd;

	if ( _sp + len > _stack_end )
	{
		_errno = rc_empty;
		return *this;
	}

	while ( len > 0 )
	{
		if ( _cache_ptr + len <= _cache_begin + hsai::page_size )
		{
			memcpy( (void *) buf, (void *) ( _cache_ptr ), len );
			_sp		   += len;
			buf		   += len;
			_cache_ptr += len;
			_errno		= rc_ok;
			return *this;
		}
		else
		{
			u64 l = _cache_begin + hsai::page_size - _cache_ptr;
			memcpy( (void *) buf, (void *) ( _cache_ptr ), len );
			buf += l;
			len -= l;
			_sp += l;
			_update_cache( (void *) ( _sp ) );
		}
	}
	_errno = rc_ok;

	return *this;
}


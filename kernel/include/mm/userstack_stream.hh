//
// Created by Li shuang ( pseudonym ) on 2024-08-07
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include <EASTL/string.h>

#include <hsai_global.hh>
#include <mem/page.hh>
#include <mem/virtual_memory.hh>
#include <tuple>

#include "mm/page_table.hh"


namespace mm
{
	using UsBufPtr	  = u8 *;
	using UsBufLen	  = u64;
	using UsRangeDesc = std::tuple<UsBufPtr, UsBufLen>;

	class UserstackStream
	{
		using PT = PageTable;

	public:

		enum RetCode
		{
			rc_ok,
			rc_fail,
			rc_not_open,
			rc_no_mem,
			rc_empty
		};

	private:

		u64 _stack_base = 0; // 用户空间起始地址
		u64 _stack_end	= 0; // 用户空间结束地址
		PT *_pt			= nullptr;
		u64 _sp			= 0; // 用户栈指针

		u8 *_cache_ptr	 = nullptr; // 缓存的页内部指针
		u8 *_cache_begin = nullptr; // 缓存的页结束地址

		RetCode _errno = rc_ok;

	public:

		UserstackStream() = default;
		UserstackStream( void *u_addr_start, u64 u_size, PT *u_pt );

		RetCode open();
		RetCode close();

		RetCode errno() { return _errno; }

		u64 sp() { return _sp; }

		UserstackStream &operator<<( UsRangeDesc &rd );
		UserstackStream &operator>>( UsRangeDesc &rd );

		template <typename T> UserstackStream &operator>>( T &d )
		{
			UsRangeDesc rd{ (u8 *) &d, sizeof( d ) };
			return *this >> rd;
		}

		template <typename T> UserstackStream &operator<<( T &d )
		{
			UsRangeDesc rd{ (u8 *) &d, sizeof( d ) };
			return *this << rd;
		}

		UserstackStream &operator>>( const char *str )
		{
			do {
				*this >> *str;
				str++;
			}
			while ( *str != 0 );
			return *this;
		}
		UserstackStream &operator<<( const char *str )
		{
			const char *p = str;
			while ( *p ) p++;
			do {
				*this << *p;
				p--;
			}
			while ( p >= str );
			return *this;
		}

		UserstackStream &operator>>( char *str )
		{
			return *this >> (const char *) str;
		}
		UserstackStream &operator<<( char *str )
		{
			return *this << (const char *) str;
		}

		UserstackStream &operator>>( eastl::string &str )
		{
			char c;
			str.clear();
			while ( 1 )
			{
				*this >> c;
				if ( c == 0 ) break;
				str.push_back( c );
			}
			return *this;
		}

		UserstackStream &operator<<( eastl::string &str )
		{
			char c = '\0';
			*this << c;
			for ( auto it = str.rbegin(); it != str.rend(); it++ ) *this << *it;
			return *this;
		}

		UserstackStream &operator+=( long move )
		{
			u8 *moveto = _cache_ptr + move;
			if ( moveto >= _cache_begin &&
				 moveto < _cache_begin + hsai::page_size )
			{
				_cache_ptr	= moveto;
				_sp		   += move;
			}
			else
			{
				_sp += move;
				_update_cache( (void *) _sp );
			}
			return *this;
		}

		UserstackStream &operator-=( long move )
		{
			move = -move;
			return ( *this += move );
		}

	private:

		int _update_cache( void *va )
		{
			u64 a = (u64) va;
			if ( hsai::is_page_align( a ) )
			{
				_cache_begin = (u8 *) hsai::k_mem->to_vir(
					_pt->walk_addr( a - hsai::page_size ) );
				_cache_ptr = _cache_begin + hsai::page_size;
			}
			else
			{
				_cache_ptr = (u8 *) hsai::k_mem->to_vir( _pt->walk_addr( a ) );
				_cache_begin =
					(u8 *) hsai::page_round_down( (ulong) _cache_ptr );
			}
			return 0;
		}
	};
} // namespace mm

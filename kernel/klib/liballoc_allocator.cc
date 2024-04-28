//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/liballoc_allocator.hh"
#include "mm/page_table.hh"

namespace kernellib
{
// ________________________________________________________________
// >>>> class AllocatorL 

	void AllocatorL::init( const char *lock_name, mm::PageTable *tbl )
	{
		_lock.init( lock_name );
		_page_table = tbl;

		for ( auto &fp : _free_pages )
			fp = nullptr;
		for ( auto &cp : _complete_pages )
			cp = 0;
	}

// -------- public interface --------

	void *AllocatorL::malloc( uint size )
	{
		_lock.acquire();

		int index = get_exponent( size );
		if ( index < ( int ) alloc_min_exponent )
			index = ( int ) alloc_min_exponent;

		boundary_tag *tag = _free_pages[ index ];
		while ( tag != nullptr )
		{
			if ( tag->real_size - sizeof( boundary_tag )
				>= size + sizeof( boundary_tag ) )
			{
				break;
			}
			tag = tag->next;
		}

		if ( tag == nullptr )
		{

		}

		return nullptr;
	}

// -------- private helper function -------- 

	int AllocatorL::get_exponent( uint size )
	{
		if ( size < alloc_min_size )
			return -1;	// Smaller than the quantum.

		int shift = alloc_min_exponent;
		while ( shift < ( int ) alloc_max_exponent )
		{
			if ( ( 1U << shift ) > size )
				break;
			shift += 1;
		}
		return shift - 1;
	}

	boundary_tag *AllocatorL::allocate_new_tag( uint size )
	{
		return nullptr;
	}

} // namespace kernellib

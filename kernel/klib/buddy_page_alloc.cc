//
// Created by Li Shuang ( pseudonym ) on 2024-05-01 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/buddy_page_alloc.hh"
#include "mm/buddy_allocator.hh"
#include "mm/page.hh"

namespace kernellib
{
	void BuddyPageAllocator::init( mm::BuddyAllocator *allocator )
	{
		_allocator = allocator;
	}

	void * BuddyPageAllocator::alloc_pages( uint cnt )
	{
		if ( _allocator )
			return _allocator->allocate_pages( cnt );
		else
			return nullptr;
	}

	void BuddyPageAllocator::free_pages( void * ptr )
	{
		if ( _allocator )
			_allocator->free_pages( ptr );
	}

} // namespace kernellib

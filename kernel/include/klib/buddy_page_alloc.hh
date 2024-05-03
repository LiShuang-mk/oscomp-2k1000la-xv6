//
// Created by Li Shuang ( pseudonym ) on 2024-05-01 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "klib/page_alloc_interface.hh"

namespace mm
{
	class BuddyAllocator;
} // namespace mm


namespace kernellib
{
	class BuddyPageAllocator : public PageAllocator
	{
	private:
		mm::BuddyAllocator *_allocator;
	public:
		BuddyPageAllocator() { _allocator = nullptr; };
		void init( mm::BuddyAllocator *allocator );

		virtual void * alloc_pages( uint cnt ) override;
		virtual void free_pages( void * ptr ) override;
	};
} // namespace kernellib

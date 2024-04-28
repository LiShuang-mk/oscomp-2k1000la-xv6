//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

/**
 * This allocator is from liballoc, but be packaged into a class.
*/

#include "types.hh"
#include "smp/lock.hh"

namespace mm
{
	class PageTable;
} // namespace mm


namespace kernellib
{
	constexpr uint alloc_max_exponent = 32;
	constexpr uint alloc_min_exponent = 8;
	constexpr uint alloc_min_size = 1U << alloc_min_exponent;

	/** This is a boundary tag which is prepended to the
	 * page or section of a page which we have allocated. It is
	 * used to identify valid memory blocks that the
	 * application is trying to free.
	 */
	struct	boundary_tag
	{
		unsigned int magic;			//< It's a kind of ...
		unsigned int size; 			//< Requested size.
		unsigned int real_size;		//< Actual size.
		int index;					//< Location in the page table.

		struct boundary_tag *split_left;	//< Linked-list info for broken pages.	
		struct boundary_tag *split_right;	//< The same.

		struct boundary_tag *next;	//< Linked list info.
		struct boundary_tag *prev;	//< Linked list info.
	};


	/// @brief 'L' means liballoc 
	class AllocatorL
	{
	private:
		smp::Lock _lock;
		mm::PageTable *_page_table;

		boundary_tag *_free_pages[ alloc_max_exponent ];
		int _complete_pages[ alloc_max_exponent ];

	public:
		AllocatorL() {};
		void init( const char *lock_name, mm::PageTable *page_table );

		void *malloc( uint size );
		void *realloc( void *ptr, uint size );
		void *calloc( uint n, uint size );
		void free( void *ptr );

	private:
		int get_exponent( uint size );
		boundary_tag *allocate_new_tag( uint size );
	};
} // namespace kernellib

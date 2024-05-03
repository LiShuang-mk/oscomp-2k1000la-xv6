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

#include "smp/lock.hh"
#include "mm/page.hh"

namespace mm
{
	class PageTable;
	class BuddyAllocator;
} // namespace mm


namespace kernellib
{

	/* A structure found at the top of all system allocated
	 * memory blocks. It details the usage of the memory chunk.
	 */
	struct L_TagMajor
	{
		struct L_TagMajor *prev;		///< Linked list information.
		struct L_TagMajor *next;		///< Linked list information.
		uint pages;						///< The number of pages in the chunk.
		uint size;						///< The number of pages in the chunk.
		uint usage;						///< The number of bytes used in the chunk.
		struct L_TagMinor *first;		///< A pointer to the first allocated memory in the chunk.	
	}__attribute__( ( __packed__ ) );


	/* This is a structure found at the beginning of all
	 * sections in a major chunk which were allocated by a
	 * malloc, calloc, realloc call.
	 */
	struct L_TagMinor
	{
		struct L_TagMinor *prev;		//< Linked list information.
		struct L_TagMinor *next;		//< Linked list information.
		struct L_TagMajor *chunk;		//< The owning chunk. A pointer to the major structure.
		uint magic;						//< A magic number to idenfity correctness.
		uint size; 						//< The size of the memory allocated. Could be 1 byte or more.
		uint req_size;					//< The size of memory requested.
	}__attribute__( ( __packed__ ) );


	/// @brief 'L' means liballoc 
	class L_Allocator
	{
	private:
		smp::Lock _lock;
		mm::BuddyAllocator * _base_allocator;

		constexpr static uint _default_page_per_chunk = 32;
		uint _page_per_chunk;			//< The number of pages to request per chunk. It is allowed to be dynamic changed but now it is not implement.

		L_TagMajor * _mem_root;			//< The root memory chunk acquired from the system.
		L_TagMajor * _best_bet;			//< The major with the most free memory.

		uint64 _cach_size;				//< Running total of allocated memory from physical memory.
		uint64 _used_size;				//< Running total of used memory requested by user.

		constexpr static uint32 _liballoc_magic_ = 0xc001c0deU;
		constexpr static uint32 _liballoc_dead = 0xdeaddeadU;

		constexpr static bool _use_align_ = true;
		using _AlignType_ = uint16;
		constexpr static uint64 _alignment_ = _use_align_ ? sizeof( _AlignType_ ) * 8 : 0;		// if open align mode, then align the pointer to 10h
		constexpr static uint64 _align_info_length_ = 16ul;										// char[16]

	public:
		L_Allocator() {};
		void init( const char *lock_name, mm::BuddyAllocator * base_alloc );

		void *malloc( uint64 size );
		void *realloc( void *ptr, uint64 size );
		void *calloc( uint n, uint64 size );
		void free( void *ptr );

	private:
		L_TagMajor * _allocate_new_chunk( uint64 size );
		void _align( void * &p );
		void _unalign( void * &p );
	};
} // namespace kernellib

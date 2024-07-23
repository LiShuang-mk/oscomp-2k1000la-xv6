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


#include "mm/page.hh"
#include "mm/virtual_page_allocator.hh"

#include <smp/spin_lock.hh>

namespace mm
{
	struct L_TagMajor;

	/* This is a structure found at the beginning of all
	* sections in a major chunk which were allocated by a
	* malloc, calloc, realloc call.
	*/
	struct L_TagMinor
	{
		L_TagMinor *prev;		//< Linked list information.
		L_TagMinor *next;		//< Linked list information.
		L_TagMajor *chunk;		//< The owning chunk. A pointer to the major structure.
		ulong 		magic;						//< A magic number to idenfity correctness.
		uint 		size; 						//< The size of the memory allocated. Could be 1 byte or more.
		uint 		req_size;					//< The size of memory requested.
	}__attribute__( ( __packed__ ) );
	static_assert( sizeof( L_TagMinor ) % 8 == 0 );

	/* A structure found at the top of all system allocated
	 * memory blocks. It details the usage of the memory chunk.
	 */
	struct L_TagMajor
	{
		L_TagMajor *prev;		///< Linked list information.
		L_TagMajor *next;		///< Linked list information.
		ulong		_reserved;	///< reserved in order to align
		uint 		size;		///< The size of the chunk (bytes).
		uint 		usage;		///< The number of bytes used in the chunk.
		L_TagMinor *first;		///< A pointer to the first allocated memory in the chunk.	
	}__attribute__( ( __packed__ ) );
	static_assert( sizeof( L_TagMajor ) % 8 == 0 );


		/// @brief 'L' means liballoc 
	class L_Allocator
	{
	private:
		hsai::SpinLock _lock;
		VirtualPageAllocator *_base_allocator;

		constexpr static uint _default_page_per_chunk = 16;
		uint _page_per_chunk;			//< The number of pages to request per chunk. It is allowed to be dynamic changed but now it is not implement.

		L_TagMajor *_mem_root;			//< The root memory chunk acquired from the system.
		L_TagMajor *_best_bet;			//< The major with the most free memory.

		uint64 _cach_size;				//< Running total of allocated memory from physical memory.
		uint64 _used_size;				//< Running total of used memory requested by user.
		uint64 _meta_size;				//< record meta data size.

		constexpr static ulong _liballoc_magic_ = 0x17ba110c17ba110cUL;
		constexpr static ulong _liballoc_dead = 0xdeaddeaddeaddeadUL;

		constexpr static bool _use_align_ = true;
		using _AlignType_ = ulong;
		constexpr static uint64 _alignment_ = _use_align_ ? sizeof( _AlignType_ ) : 0;		// if open align mode, then align the pointer to b'1000

	public:
		L_Allocator() {};
		void init( const char *lock_name, VirtualPageAllocator *base_alloc );

		void *malloc( i64 size );
		void *realloc( void *ptr, uint64 size );
		void *calloc( uint n, uint64 size );
		void free( void *ptr );

		u64 get_cached_size() { return _cach_size; }
		u64 get_used_size() { return _used_size; }
		u64 get_meta_size() { return _meta_size; }

	private:
		L_TagMajor *_allocate_new_chunk( uint64 size );
		// void _align( void *&p );
		// void _unalign( void *&p );
		i64 _size_align( i64 size ) { return ( i64 ) ( ( u64 ) size + _alignment_ - 1 ) & ( ~( _alignment_ - 1 ) ); };

	public:/*debug*/
		void debug_print();
	};

} // namespace klib

//
// Created by Li Shuang ( pseudonym ) on 2024-04-30 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/global_operator.hh"
#include "mm/heap_memory_manager.hh"

void * operator new ( uint64 size )
{
	log_info( "new with size %d", size );
	return mm::k_hmm.allocate( size );
}
void * operator new[] ( uint64 size )
{
	log_info( "new with size %d", size );
	return mm::k_hmm.allocate( size );
}
void operator delete ( void * p ) noexcept
{
	log_info( "delete 0x%p", p );
	mm::k_hmm.free( p );
}
void operator delete[] ( void * p ) noexcept
{
	log_info( "delete[] 0x%p", p );
	mm::k_hmm.free( p );
}
void operator delete( void * p, uint64 size ) noexcept
{
	log_info( "delete 0x%p with size %d", p, size );
	mm::k_hmm.free( p );
}
void operator delete[] ( void * p, uint64 size ) noexcept
{
	log_info( "delete[] 0x%p with size %d", p, size );
	mm::k_hmm.free( p );
}
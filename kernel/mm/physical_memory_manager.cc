//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/physical_memory_manager.hh"
#include "klib/common.hh"
#include "mm/page.hh"

PhysicalMemoryManager k_pmm;

void PhysicalMemoryManager::init( const char *name, uint64 ram_base, uint64 ram_end )
{
	_lock.init( name );
	_ram_base = ram_base;
	_ram_end = ram_end;
	_free_list = 0;
	_free_range( ( void* ) _ram_base, ( void * ) _ram_end );
}

void PhysicalMemoryManager::_free_range( void *pa_start, void *pa_end )
{
	char *p;
	p = ( char * ) page_round_up( ( uint64 ) pa_start );
	for ( ; p + pg_size <= ( char * ) pa_end; p += pg_size )
		_free_page( p );
}

void PhysicalMemoryManager::_free_page( void *pa )
{
	struct PageHead *r;

	if ( ( ( uint64 ) pa % pg_size ) != 0 || ( uint64 ) pa < _ram_base || ( uint64 ) pa >= _ram_end )
		panic( "free page" );

	  // Fill with junk to catch dangling refs.
	memset( pa, 1, pg_size );

	r = ( struct PageHead* ) pa;

	_lock.acquire();
	r->_next = _free_list;
	_free_list = r;
	_lock.release();
}

void *PhysicalMemoryManager::_alloc_page()
{
	struct PageHead *r;

	_lock.acquire();
	r = _free_list;
	if ( r )
		_free_list = r->_next;
	_lock.release();

	if ( r )
		memset( ( char* ) r, 5, pg_size );

	return 0;
}

void PhysicalMemoryManager::free_page( void *pa )
{
	_free_page( pa );
}

void *PhysicalMemoryManager::alloc_page()
{
	return _alloc_page();
}
//
// Created by Li Shuang ( pseudonym ) on 2024-05-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include <cxxabi.h>

#include "klib/__cxx_abi.hh"
#include "klib/common.hh"

namespace __cxxabiv1
{
	extern "C" {

/* **************** cxa guard **************** */

		int __cxa_guard_acquire( __guard * g )
		{
			log_info( "cxa guard acquire" );
			return !*( char * ) ( g );
		}

		void __cxa_guard_release( __guard *g )
		{
			log_info( "cxa guard release" );
			*( char * ) g = 1;
		}

		void __cxa_guard_abort( __guard * )
		{
			log_panic( "cxa guard abort" );
		}

/* **************** cxa at_exit **************** */

		atexit_func_entry_t __atexit_funcs[ ATEXIT_MAX_FUNCS ];
		atexit_func_entry_t __atexit_func_entry_free_list;
		atexit_func_entry_t __atexit_func_entry_busy_list;

		uarch_t __atexit_func_count = 0;

		void __init_atexit_func_entry( void )
		{
			__atexit_func_entry_busy_list._next_entry = nullptr;
			__atexit_func_entry_busy_list._prev_entry = nullptr;

			__atexit_func_entry_free_list._next_entry = &__atexit_func_entry_free_list;
			__atexit_func_entry_free_list._prev_entry = &__atexit_func_entry_free_list;

			for ( int i = 0; i < ATEXIT_MAX_FUNCS; i++ )
			{
				__insert_atexit_func_entry_list( &__atexit_func_entry_free_list, &__atexit_funcs[ i ] );
			}
		}

		/****************************************************************************
		 * about atexit abi, refer to:
		 * 1. https://itanium-cxx-abi.github.io/cxx-abi/abi.html#dso-dtor-runtime-api
		 * 2. https://wiki.osdev.org/C%2B%2B#GCC
		 ****************************************************************************/

		int __cxa_atexit( void ( *f )( void * ), void *objptr, void *dso )
		{
			log_info( ">>>> __cxa_atexit called" );
			if ( __atexit_func_count >= ATEXIT_MAX_FUNCS )
			{
				return -1;
			}
			if ( __atexit_func_entry_list_is_empty( &__atexit_func_entry_free_list ) )
			{
				log_panic(
					"atexit: no func entry to use but __atexit_func_count not has a valid value,\n"
					"        which makes code executed at here to avoid bad result."
				);
			}

			atexit_func_entry_t* entry = __atexit_func_entry_free_list._next_entry;
			__remove_atexit_func_entry_list( entry );

			entry->destructor_func = f;
			entry->obj_ptr = objptr;
			entry->dso_handle = dso;
			__atexit_func_count++;
			return 0;
		};

		void __cxa_finalize( void *d )
		{
			atexit_func_entry_t* fentry = __atexit_func_entry_busy_list._next_entry;
			log_info(
				">>>> __cxa_finalize called"
				">>>> d is %p",
				d
			);
			if ( !d )
			{
				while ( fentry != &__atexit_func_entry_busy_list )
				{
					atexit_func_entry_t* next_entry = fentry->_next_entry;
					if ( fentry->destructor_func )
					{
						( *fentry->destructor_func )( fentry->obj_ptr );
						__remove_atexit_func_entry_list( fentry );
						__insert_atexit_func_entry_list( &__atexit_func_entry_free_list, fentry );
						--__atexit_func_count;
					}
					fentry = next_entry;
				}
				return;
			}

			while ( fentry != &__atexit_func_entry_busy_list )
			{
				atexit_func_entry_t* next_entry = fentry->_next_entry;
				if ( fentry->dso_handle == d )
				{
					( *fentry->destructor_func )( fentry->obj_ptr );
					__remove_atexit_func_entry_list( fentry );
					__insert_atexit_func_entry_list( &__atexit_func_entry_free_list, fentry );
					--__atexit_func_count;
				}
				fentry = next_entry;
			}
		};

	}
} // namespace __cxxabiv1

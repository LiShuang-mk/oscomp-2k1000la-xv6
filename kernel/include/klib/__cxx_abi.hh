//
// Created by Li Shuang ( pseudonym ) on 2024-05-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

namespace __cxxabiv1
{
	extern "C" {

#define ATEXIT_MAX_FUNCS 128

		typedef unsigned uarch_t;

		struct atexit_func_entry_t
		{
			/*
			* Each member is at least 4 bytes large. Such that each entry is 12bytes.
			* 128 * 12 = 1.5KB exact.
			**/
			void ( *destructor_func )( void * );
			void *obj_ptr;
			void *dso_handle;

			// link list
			atexit_func_entry_t * _prev_entry;
			atexit_func_entry_t * _next_entry;
		};

		// int __cxa_atexit( void ( *f )( void * ), void *objptr, void *dso );
		// void __cxa_finalize( void *f );

		void __init_atexit_func_entry( void );
		inline void __insert_atexit_func_entry_list( atexit_func_entry_t* list, atexit_func_entry_t* entry )
		{
			entry->_prev_entry = list;
			entry->_next_entry = list->_next_entry;
			list->_next_entry->_prev_entry = entry;
			list->_next_entry = entry;
		}
		inline void __remove_atexit_func_entry_list( atexit_func_entry_t* entry )
		{
			if ( entry->_next_entry )
				entry->_next_entry->_prev_entry = entry->_prev_entry;
			if ( entry->_prev_entry )
				entry->_prev_entry->_next_entry = entry->_next_entry;
			entry->_next_entry = entry->_prev_entry = nullptr;
		}
		inline bool __atexit_func_entry_list_is_empty( atexit_func_entry_t* list )
		{
			return ( list->_next_entry == list );
		}
	}
} // namespace __cxxabiv1


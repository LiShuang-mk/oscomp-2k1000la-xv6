//
// Created by Li Shuang ( pseudonym ) on 2024-04-30 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

// #include <stddef.h>

#include "types.hh"

extern void * operator new( uint64 size );
extern void * operator new[]( uint64 size );
extern void operator delete( void * p ) noexcept;
extern void operator delete( void * p, uint64 size ) noexcept;
extern void operator delete[]( void * p ) noexcept;
extern void operator delete[]( void * p, uint64 size ) noexcept;

// inline void *operator new(size_t, void *p)     throw() { return p; }  
// inline void *operator new[](size_t, void *p)   throw() { return p; }
// inline void  operator delete  (void *, void *) throw() { };
// inline void  operator delete[](void *, void *) throw() { };

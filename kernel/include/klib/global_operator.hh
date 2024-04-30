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

#include <new>

extern void * operator new( uint64 size );
extern void * operator new[]( uint64 size );
extern void operator delete( void * p ) noexcept;
extern void operator delete( void * p, uint64 size ) noexcept;
extern void operator delete[]( void * p ) noexcept;
extern void operator delete[]( void * p, uint64 size ) noexcept;

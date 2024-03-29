//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

struct PageHead;

class PhysicalMemoryManager
{
private:
	smp::Lock _lock;
	PageHead *_free_list = nullptr;
	uint64 _ram_base;
	uint64 _ram_end;

public:
	PhysicalMemoryManager() {};
	void init( const char *name, uint64 ram_base, uint64 ram_end );
	void free_page( void *pa );
	void *alloc_page();

private:
	void _free_range( void *pa_start, void *pa_end );
	void _free_page( void *pa );
	void *_alloc_page();
};

extern PhysicalMemoryManager k_pmm;
//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "mm/physical_memory_manager.hh"
#include "mm/page_table.hh"

class VirtualMemoryManager
{
private:

public:
	static uint64 kstack_vm_from_gid( uint gid );

public:
	VirtualMemoryManager() {};
	void init();

};


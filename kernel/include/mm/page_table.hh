//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

class PageTable
{
private:
	uint64 _base_addr;

public:
	PageTable() {};
	void set_base( uint64 addr ) { _base_addr = addr; }
	uint64 get_base() { return _base_addr; }
};
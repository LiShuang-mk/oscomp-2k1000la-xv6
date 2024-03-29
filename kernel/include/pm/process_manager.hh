//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

class ProcessManager
{
private:
	smp::Lock _pid_lock;
	smp::Lock _wait_lock;

public:
	ProcessManager() = default;
	void init( const char *pid_lock_name, const char *wait_lock_name );
};

extern ProcessManager k_pm;
//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "smp/lock.hh"
#include "hal/cpu.hh"
#include "klib/common.hh"

using namespace smp;

Lock::Lock() {}

void Lock::init( const char *name )
{
	this->name = name;
	this->locked = 0;
	this->cpu = 0;
}

void Lock::acquire()
{
	loongarch::Cpu::push_intr_off();
	if ( is_held() )
		log_panic( "lock acquire: repeat" );
	while ( __sync_lock_test_and_set( &locked, 1 ) != 0 )
		;
	__sync_synchronize();
	cpu = loongarch::Cpu::get_cpu();
}

void Lock::release()
{
	if ( !is_held() )
		log_panic("lock release: not hold lock"); 
	cpu = 0;
	__sync_synchronize();
	__sync_lock_release( &locked );
	loongarch::Cpu::pop_intr_off();
}

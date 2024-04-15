#include "console.hh"
#include "hal/loongarch.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/cpu.hh"
#include "tm/timer_manager.hh"
#include "im/exception_manager.hh"
#include "im/interrupt_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include "pm/process_manager.hh"
#include "pm/shmmanager.hh"
#include "klib/printer.hh"
#include "klib/common.hh"

// entry.S needs one stack per CPU.
__attribute__( ( aligned( 16 ) ) ) char stack0[ 4096 * NUMCPU ];

int main()
{
	if ( loongarch::Cpu::read_tp() == 0 )
	{
		// console init 
		k_console.init( "console" );

		// printf init 
		kernellib::k_printer.init( &k_console, "printer" );
		log__info( "Hello World!\n" );

		// physical memory init 
		mm::k_pmm.init( "physical memory manager",
			loongarch::qemuls2k::memory::mem_start,
			loongarch::qemuls2k::memory::mem_end );
		log__info( "pmm init" );

		// process init 
		pm::k_pm.init( "next pid", "wait lock" );
		log__info( "pm init" );

		// vm init 
		mm::k_vmm.init( "virtual memory manager " );
		log__info( "vm init" );

		// timer init 
		tm::k_tm.init( "timer manager" );
		log__info( "tm init" );

		// exception init 
		im::k_em.init( "exception manager " );
		log__info( "em init" );

		// interrupt init 
		im::k_im.init( "interrupt init" );
		log__info( "im init" );

		// sharemem init
		pm::k_shmManager.init( "shm lock" );
		log__info( "shm init" );
		// uint32 apbh[ 64 ];
		// uint64 addr = ( ( 0xFE0UL << 28 ) | ( 0x0UL << 16 ) | ( 0x2UL << 11 ) | ( 0x0UL << 8 ) );
		// printf( "addr: \n%p\n", addr | loongarch::qemuls2k::dmwin::win_1 );
		// printf( "head: \n" );
		// volatile uint32 * p = ( volatile uint32 * ) ( addr | loongarch::qemuls2k::dmwin::win_1 );
		// for ( int i = 0; i < 16; i++, p++ )
		// 	apbh[ i ] = *p;
		// for ( int i = 0; i < 16; i++ )
		// 	printf( "%x\n", apbh[ i ] );

		while ( 1 ); // stop here
	}
	else
		while ( 1 );

	return 0;
}
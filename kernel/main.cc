#define NCPU 1 

#include "console.hh"
#include "hal/loongarch.hh"
#include "hal/cpu.hh"
#include "mm/physical_memory_manager.hh"
#include "pm/process_manager.hh"
#include "klib/printer.hh"
#include "klib/common.hh"

// entry.S needs one stack per CPU.
__attribute__( ( aligned( 16 ) ) ) char stack0[ 4096 * NCPU ];

int main()
{
	if ( loongarch::Cpu::read_tp() == 0 )
	{
		// console init 
		k_console.init( "console" );

		// printf init 
		kernellib::k_printer.init( &k_console, "printer" );
		printf( "Hello World!\n" );

		// physical memory init 
		mm::k_pmm.init( "physical memory manager",
			loongarch::qemuls2k::memory::mem_start,
			loongarch::qemuls2k::memory::mem_end );
		printf( "pmm init\n" );

		// process init 
		pm::k_pm.init( "next pid", "wait lock" );
		printf( "pm init\n" );

		

		log__info( "测试info" );
		log__warn( "测试warn" );
		log_error( "测试error" );
		log_panic( "中文panic" );

		while ( 1 ); // stop here
	}
	else
		while ( 1 );

	return 0;
}
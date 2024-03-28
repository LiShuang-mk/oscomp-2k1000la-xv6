#define NCPU 1 

#include "console.hh"
#include "hal/loongarch.hh"
#include "hal/cpu.hh"
#include "klib/printer.hh"
#include "klib/common.hh"

// entry.S needs one stack per CPU.
__attribute__( ( aligned( 16 ) ) ) char stack0[ 4096 * NCPU ];

int main()
{
	if ( loongarch::Cpu::read_tp() == 0 )
	{
		k_console.init( "console" );
		kernellib::k_printer.init( &k_console, "printer" );
		printf( "Hello World!" );
		while ( 1 ); // stop here
	}
	else
		while ( 1 );

	return 0;
}
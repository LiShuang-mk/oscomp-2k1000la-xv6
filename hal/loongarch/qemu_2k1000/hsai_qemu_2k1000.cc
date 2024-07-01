//
// Created by Li Shuang ( pseudonym ) on 2024-06-24 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "la_cpu.hh"
#include "qemu_2k1000.hh"

#include <hsai_global.hh>
#include <uart/uart_ns16550.hh>

namespace loongarch
{
	namespace qemu2k1000
	{
		hsai::UartNs16550 debug_uart;
	} // namespace qemu2k1000

} // namespace loongarch


namespace hsai
{
	void hardware_abstract_init( void )
	{
		loongarch::cpu_init();

		new ( &loongarch::qemu2k1000::debug_uart ) UartNs16550(
			( void* ) (
				( uint64 ) loongarch::qemu2k1000::UartAddr::uart0
				| ( uint64 ) loongarch::qemu2k1000::dmwin::win_1
				) );
		register_debug_uart( &loongarch::qemu2k1000::debug_uart );
	}

} // namespace hsai

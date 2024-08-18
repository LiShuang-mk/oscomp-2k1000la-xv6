//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "qemu_2k1000.hh"
#include "interrupt_manager.hh"
#include "la_cpu.hh"

#include <hsai_global.hh>
#include <hsai_defs.h>
#include <device_manager.hh>
#include <uart/uart_ns16550.hh>
#include <ata/ahci_driver.hh>
#include <hsai_log.hh>

namespace loongarch
{
	namespace qemu2k1000
	{
		InterruptManager k_im;

		InterruptManager::InterruptManager( const char * lock_name )
		{
			_lock.init( lock_name );

			write_itr_cfg( ItrCfg::itr_clr_l, ~0x0U );
			write_itr_cfg( ItrCfg::itr_clr_h, ~0x0U );

			write_itr_cfg( ItrCfg::itr_edg_l, 0x0U );
			write_itr_cfg( ItrCfg::itr_edg_h, 0x0U );
			write_itr_cfg( ItrCfg::itr_pol_l, 0x0U );
			write_itr_cfg( ItrCfg::itr_pol_h, 0x0U );

			write_itr_cfg( ItrCfg::itr_bou_l, 0x0U );
			write_itr_cfg( ItrCfg::itr_bou_h, 0x0U );
			write_itr_cfg( ItrCfg::itr_aut_l, 0x0U );
			write_itr_cfg( ItrCfg::itr_aut_h, 0x0U );

			write_itr_cfg(
				ItrCfg::itr_enr_l,
				ItrCfg::itr_bit_uart0_m |
				ItrCfg::itr_bit_sata_m
			);
			write_itr_cfg_8b(
				ItrCfg::itr_route_uart0,
				itr_route_xy( 0, 2 )
			);
			write_itr_cfg_8b(
				ItrCfg::itr_route_sata,
				itr_route_xy( 0, 3 )
			);

			if ( register_interrupt_manager( this ) < 0 )
			{
				hsai_panic( "register interrupt manager fail." );
			}

			intr_init();

			Cpu * cpu = Cpu::get_la_cpu();
			cpu->intr_on();
		}

		void InterruptManager::intr_init()
		{
			_uart0 = ( hsai::UartNs16550 * ) hsai::k_devm.get_char_device( DEFAULT_DEBUG_CONSOLE_NAME );
			if ( _uart0 == nullptr )
			{
				hsai_panic( "couldn't find console device" );
			}

			_sata = ( hsai::AhciDriver * ) hsai::k_devm.get_device( "AHCI driver" );
			if ( _sata == nullptr )
			{
				hsai_panic( "couldn't find AHCI device" );
			}
		}

		void InterruptManager::clear_uart0_intr()
		{
			hsai_trace(
				"before clear uart0 intrn\n"
				"itr status : %p\n"
				"ena status : %p\n",
				read_itr_cfg(
					ItrCfg::itr_isr_l ),
				read_itr_cfg(
					ItrCfg::itr_esr_l )
			);

			// dev::k_console.handle_uart_intr();

			hsai_trace(
				"after clear uart0 intrn\n"
				"itr status : %p\n"
				"ena status : %p\n",
				read_itr_cfg(
					ItrCfg::itr_isr_l ),
				read_itr_cfg(
					ItrCfg::itr_esr_l )
			);
		}



		int InterruptManager::handle_dev_intr()
		{

			uint32 irq = *( volatile uint32 * ) ( itr_isr_l );

			if ( irq & itr_bit_uart0_m )
			{
				// uartintr();
				// hsai_warn( "uart intr not implement" );
				_uart0->handle_intr();
			}
			if ( irq & itr_bit_sata_m )
			{
				_sata->handle_intr();
			}
			// if ( irq )
			// {
			// 	hsai_error( "unexpected interrupt irq=%d\n", irq );

			// 	// apic_complete( irq );
			// 	// extioi_complete( irq );
			// }

			return 1;
		}
	} // namespace qemu2k1000

} // namespace im

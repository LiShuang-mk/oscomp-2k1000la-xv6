//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/console.hh"
#include "hal/qemu_ls2k.hh"
#include "im/interrupt_manager.hh"
#include "klib/common.hh"

namespace im
{
	InterruptManager k_im;

	void InterruptManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_clr_l, ~0x0U );
		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_clr_h, ~0x0U );

		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_edg_l, 0x0U );
		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_edg_h, 0x0U );
		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_pol_l, 0x0U );
		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_pol_h, 0x0U );

		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_bou_l, 0x0U );
		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_bou_h, 0x0U );
		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_aut_l, 0x0U );
		loongarch::qemuls2k::write_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_aut_h, 0x0U );

		loongarch::qemuls2k::write_itr_cfg(
			loongarch::qemuls2k::ItrCfg::itr_enr_l,
			loongarch::qemuls2k::ItrCfg::itr_bit_uart0_m |
			loongarch::qemuls2k::ItrCfg::itr_bit_sata_m
		);
		loongarch::qemuls2k::write_itr_cfg(
			loongarch::qemuls2k::ItrCfg::itr_route_uart0,
			loongarch::qemuls2k::itr_route_xy( 0, 2 )
		);
		loongarch::qemuls2k::write_itr_cfg(
			loongarch::qemuls2k::ItrCfg::itr_route_sata,
			loongarch::qemuls2k::itr_route_xy( 0, 3 )
		);
	}

	void InterruptManager::clear_uart0_intr()
	{
		log_trace(
			"before clear uart0 intrn\n"
			"itr status : %p\n"
			"ena status : %p\n",
			loongarch::qemuls2k::read_itr_cfg(
				loongarch::qemuls2k::ItrCfg::itr_isr_l ),
			loongarch::qemuls2k::read_itr_cfg(
				loongarch::qemuls2k::ItrCfg::itr_esr_l )
		);

		dev::k_console.handle_uart_intr();

		log_trace(
			"after clear uart0 intrn\n"
			"itr status : %p\n"
			"ena status : %p\n",
			loongarch::qemuls2k::read_itr_cfg(
				loongarch::qemuls2k::ItrCfg::itr_isr_l ),
			loongarch::qemuls2k::read_itr_cfg(
				loongarch::qemuls2k::ItrCfg::itr_esr_l )
		);
	}

	int InterruptManager::handle_dev_intr()
	{

		uint32 irq = *( volatile uint32 * ) ( loongarch::qemuls2k::itr_isr_l );

		if ( irq & ( loongarch::qemuls2k::ItrCfg::itr_bit_uart0_m ) )
		{
			// uartintr();
			log_warn( "uart intr not implement" );
		}
		else if ( irq )
		{
			printf( "unexpected interrupt irq=%d\n", irq );

			// apic_complete( irq );
			// extioi_complete( irq );
		}

		return 1;
	}
} // namespace im

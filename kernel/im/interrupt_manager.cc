//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/qemu_ls2k.hh"
#include "im/interrupt_manager.hh"

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
			loongarch::qemuls2k::ItrCfg::itr_bit_uart0_m
		);
		loongarch::qemuls2k::write_itr_cfg(
			loongarch::qemuls2k::ItrCfg::itr_route_uart0,
			loongarch::qemuls2k::itr_route_xy( 0, 2 )
		);
	}
} // namespace im

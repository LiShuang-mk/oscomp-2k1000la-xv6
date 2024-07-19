//
// Created by Li Shuang ( pseudonym ) on 2024-07-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "smp/spin_lock.hh"
#include "virtual_device.hh"
#include "ahci_port_driver.hh"

namespace hsai
{
	class AhciDriver : public VirtualDevice
	{
	private:
		SpinLock _lock;
		AhciPortDriver _port_drivers[ ahci_max_port_num ];
		int _port_num = 0;												// 初始化成功的端口数量
		u32 _port_bitmap = 0;											// 初始化成功的端口位图
		AhciMemReg * _regs = nullptr;

	public:
		AhciDriver() = default;
		AhciDriver( const char * lock_name, void * base_addr );
		virtual DeviceType type() override { return DeviceType::dev_other; }
		virtual int handle_intr() override;
	};

} // namespace hsai

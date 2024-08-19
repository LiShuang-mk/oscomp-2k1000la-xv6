//
// Created by Li Shuang ( pseudonym ) on 2024-07-17
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include "ahci_port_driver.hh"
#include "smp/spin_lock.hh"
#include "virtual_device.hh"

namespace hsai
{
	class AhciDriver : public VirtualDevice
	{
	private:

		SpinLock	   _lock;
		AhciPortDriver _port_drivers[ahci_max_port_num];
		int			   _port_num	= 0; // 初始化成功的端口数量
		u32			   _port_bitmap = 0; // 初始化成功的端口位图
		AhciMemReg *   _regs		= nullptr;

	public:

		AhciDriver() = default;
		AhciDriver( const char * lock_name, void * base_addr );
		virtual DeviceType type() override { return DeviceType::dev_other; }
		virtual bool	   read_ready() override { return false; }
		virtual bool	   write_ready() override { return false; }

		virtual int handle_intr() override;

	public:

		/// @brief 向设备发送idnetify命令，获取设备元信息
		void identify_device();

	private:

		int _analyze_identify_data( uint port_id, word * id_data );

		enum
		{
			mbr_none   = 0,
			mbr_chk_ok = 1,
			mbr_gpt	   = -10,
			mbr_fat32  = 2,
			mbr_ext	   = 3
		};
		int _check_mbr_partition( u8 * mbr, uint port_id );
	};

} // namespace hsai

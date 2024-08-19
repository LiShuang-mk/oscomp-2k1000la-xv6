//
// Created by Li Shuang ( pseudonym ) on 2024-08-17
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include <smp/spin_lock.hh>
#include <virtual_device.hh>

#include "ahci_port_driver_ls.hh"

namespace loongarch
{
	namespace qemu2k1000
	{
		/// @brief 用于2k1000LA星云板的AHCI驱动器
		/// @details 实际上qemu-2k1000不需要用这个特定的驱动器
		///          但是在2k1000LA星云板上，SATA-DMA的访存有些奇怪
		///          这个驱动器是为其特殊的访存开发的
		///          注意 >> 这个驱动器需要配合Uboot使用
		///               >> 必须让Uboot使用scsi reset进行重置
		class AhciDriverLs : public hsai::VirtualDevice
		{
		private:

			hsai::SpinLock	  _lock;
			AhciPortDriverLs  _port_drivers[hsai::ahci_max_port_num];
			int				  _port_num	   = 0; // 初始化成功的端口数量
			u32				  _port_bitmap = 0; // 初始化成功的端口位图
			hsai::AhciMemReg *_regs		   = nullptr;

		public:

			virtual hsai::DeviceType type() override { return hsai::DeviceType::dev_other; }

			virtual bool read_ready() override { return false; }
			virtual bool write_ready() override { return true; }
			virtual int	 handle_intr() override;

		public:

			AhciDriverLs() = default;
			AhciDriverLs( const char *lock_name, void *base_addr );

			void identify_device();

		private:

			int _analyze_identify_data( uint port_id, u16 *id_data );

			enum
			{
				mbr_none   = 0,
				mbr_chk_ok = 1,
				mbr_gpt	   = -10,
				mbr_fat32  = 2,
				mbr_ext	   = 3
			};
			int _check_mbr_partition( u8 *mbr, uint port_id );
		};
	} // namespace qemu2k1000

} // namespace loongarch

//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <smp/spin_lock.hh>
#include <intr/virtual_interrupt_manager.hh>

namespace hsai
{
	class UartNs16550;
	class AhciDriver;
} // namespace hsai


namespace loongarch
{
	namespace qemu2k1000
	{
		class InterruptManager : public hsai::VirtualInterruptManager
		{
		private:
			hsai::SpinLock _lock;
			hsai::UartNs16550 * _uart0 = nullptr;
			hsai::AhciDriver * _sata = nullptr;

		public:
			InterruptManager() = default;
			InterruptManager( const char *lock_name );
			void intr_init();

			void clear_uart0_intr();

			virtual int handle_dev_intr() override;
		};

		extern InterruptManager k_im;

	} // namespace qemu2k1000

} // namespace loongarch

//
// Created by Li shuang ( pseudonym ) on 2024-04-14 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

namespace ata
{
	namespace sata
	{
		struct HbaCmdTbl;
	} // namespace sata

} // namespace ata

extern void default_call_back();

namespace dev
{
	namespace ahci
	{
		class AhciController
		{
		private:
			smp::Lock _lock;
			struct ata::sata::HbaCmdTbl *_cmd_tbl = nullptr;
			void *_pr = nullptr;
			int _is_idtf = false;

			using callback_t = void( * )( void );

			callback_t test_call_back = default_call_back;

		public:
			AhciController() = default;
			void init( const char *lock_name );

			/// @brief 发送identify命令
			/// @details 这个函数可以作为AHCI给SATA发送命令的编程模板
			///          to do:
			///          这里使用了默认的0号命令槽，但实际上有可能0号槽在忙；
			///          并且这个函数在SMP中是不安全的；
			///          1. 如何在SataDriver中增添自动获取可用的命令槽？
			///          2. 如何通过lock使得这个函数是SMP安全的？
			///          此外，这里使用指定port的方式发送命令，但实际上上层
			///          很可能并不知道port对应的设备是什么
			///          to do:
			///          3. 如何将port抽象给上层？使得上层只需要告诉AHCI设备
			///             是什么就可以发送到正确的port上
			/// @param port 端口号
			/// @param buffer 接受数据的缓存基地址，请注意buffer由调用者来管理
			/// @param len buffer缓存的大小（不小于512byte）
			void isu_cmd_identify( uint port, void *buffer, uint len );

			void isu_cmd_read_dma( uint64 lba );

			/// @brief 尽管当前这个中断处理函数仍在测试中使用，但这个函数在
			///        正式的代码中应该被废弃
			void simple_intr_handle();

			/// @brief 正式的中断处理函数应当在此处使用
			void intr_handle();

			void set_intr_handler( callback_t f ) { test_call_back = f; }

		};

		extern AhciController k_ahci_ctl;
	} // namespace ahci

} // namespace dev

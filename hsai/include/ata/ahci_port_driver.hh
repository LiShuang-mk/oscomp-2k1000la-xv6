//
// Created by Li Shuang ( pseudonym ) on 2024-07-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "smp/spin_lock.hh"
#include "ata/ahci.hh"
#include "ata/sata.hh"
#include "block_device.hh"

#include <kernel/klib/function.hh>

namespace hsai
{
	/*****************************************************************
	 * 通用设备驱动 - AHCI 驱动
	 * >> 本驱动参考 Intel Searial ATA Advanced Host Controller Interface
	 * >> revision 1.3.1 实现，任何相关实现均可参考该文档
	 * ！！注意！！
	 * >> 这个驱动只是尽可能地驱动一个简单的 SATA 设备，实现基本的
	 * >> Identify, DMA read, DMA write 命令。SATA 端口复用，设备休眠
	 * >> 等在SATA协议中规定的更复杂的功能一概不论。请使用时注意鉴别
	 * >> 各方法功能。
	 *****************************************************************/

	const char ahci_port_driver_name_template[] = "AHCI port xx";
	constexpr void ahci_port_change_name_number( char * name, int num )
	{
		char c;
		c = ( char ) ( num / 10 ) + '0';
		name[ sizeof ahci_port_driver_name_template - 3 ] = c;
		c = ( char ) ( num % 10 ) + '0';
		name[ sizeof ahci_port_driver_name_template - 2 ] = c;
	}

	class AhciPortDriver : public BlockDevice
	{
	private:
		hsai::SpinLock _lock;
		const char _dev_name[ sizeof ahci_port_driver_name_template ] = "";
		int _port_id = 0;
		AhciPortReg * _regs = nullptr;
		AhciCmdList * _cmd_ls = nullptr;
		AhciRevFis * _rev_fis = nullptr;
		AhciCmdTbl *_cmd_tbl = nullptr;		// 实际上命令槽不只一个，这里简化设计，只使用一个命令槽
		ulong _block_size = 0;
		int _is_idtf = false;

		std::function<void( void )> _call_back;

		std::function<void( void )> _call_back_function[ ahci_max_port_num ][ ahci_max_cmd_slot ];

	public:
		virtual long get_block_size() override;
		virtual int read_blocks_sync( long start_block, long block_count, void * buf_list, int buf_count ) override;
		virtual int read_blocks( long start_block, long block_count, void * buf_list, int buf_count ) override;
		virtual int write_blocks_sync( long start_block, long block_count, void * buf_list, int buf_count ) override;
		virtual int write_blocks( long start_block, long block_count, void * buf_list, int buf_count ) override;

	public:
		AhciPortDriver() = default;
		AhciPortDriver(
			const char * lock_name,
			int port_id,
			void * base_addr,
			void * cmd_list,
			void * fis_base
		);
		void init( const char *lock_name );

		/// @brief 发送identify命令
		/// @details 这个函数可以作为AHCI给SATA发送命令的编程模板
		///          to do:
		///          这里使用了默认的0号命令槽，但实际上有可能0号槽在忙；
		///          并且这个函数在SMP中是不安全的；
		///          1. 如何在SataDriver中增添自动获取可用的命令槽？
		///          2. 如何通过lock使得这个函数是SMP安全的？
		///             此外，这里使用指定port的方式发送命令，但实际上上层
		///             很可能并不知道port对应的设备是什么
		///          to do:
		///          3. 如何将port抽象给上层？使得上层只需要告诉AHCI设备
		///             是什么就可以发送到正确的port上
		/// @param port 端口号
		/// @param buffer 接受数据的缓存基地址，请注意buffer由调用者来管理
		/// @param len buffer缓存的大小（不小于512byte）
		/// @param callback_handler 回调函数指针，为空指针时使用默认回调函数
		void isu_cmd_identify( uint port, void *buffer, uint len, std::function<void( void )> callback_handler );

		/// @brief 向SATA设备发送read DMA命令
		/// @param port 端口号
		/// @param lba 逻辑扇区号
		/// @param len 读取数据的长度(byte)，按逻辑扇区大小对齐（通常是512bytes）
		/// @param prd_cnt 指定PRD数量
		/// @param set_prd_handler 设置PRD的回调函数
		/// @param callback_handler 命令结束后中断的回调函数
		void isu_cmd_read_dma(
			uint port,
			uint64 lba,
			uint64 len,
			uint prd_cnt,
			std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
			std::function<void( void )> callback_handler );

		void isu_cmd_write_dma(
			uint port,
			uint64 lba,
			uint64 len,
			uint prd_cnt,
			std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
			std::function<void( void )> callback_handler );

		/// @brief 尽管当前这个中断处理函数仍在测试中使用，但这个函数在
		///        正式的代码中应该被废弃
		// void simple_intr_handle();

		/// @brief 正式的中断处理函数应当在此处使用
		void intr_handle();

		// void set_intr_handler( callback_t f ) { test_call_back = f; }

	// for debugging 
	public:

	private:
		void fill_fis_h2d_lba( SataFisRegH2D *fis, uint64 lba );
	};

} // namespace hsai

//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hal/sata/hba_param.hh"
#include "hal/sata/hba_port.hh"
#include "klib/common.hh"

#include <smp/spin_lock.hh>

namespace ata
{
	namespace sata
	{
		struct HbaMemReg;
		struct HbaPortReg;
		struct HbaCmdList;
		struct HbaCmdHeader;
		struct HbaCmdTbl;
		struct HbaRevFis;
		struct FisRegH2D;
	} // namespace sata

} // namespace ata


namespace dev
{
	namespace pci
	{
		class PciDriver;
	} // namespace pci


	namespace ahci
	{
		class AhciController;
	} // namespace ahci


	namespace sata
	{
		class SataDriver
		{
			friend ahci::AhciController;
			friend pci::PciDriver;
		protected:
			hsai::SpinLock _lock;
			ata::sata::HbaMemReg *_hba_mem_reg = nullptr;
			ata::sata::HbaPortReg *_hba_port_reg[ ata::sata::max_port_num ];
			uint _port_num = 0;
			// 这里本来应该要保留 port map，
			// 简化处理为所有的可用port都是在最低位连续排列
			// uint32 _port_map = 0;
			uint32 _cap_cache = 0;

			ata::sata::HbaCmdList *_port_cmd_lst_base[ ata::sata::max_port_num ];
			uint8 _port_cmd_slot_num[ ata::sata::max_port_num ];
			ata::sata::HbaRevFis *_port_rec_fis_base[ ata::sata::max_port_num ];

			uint _logical_sector_size;

		public:
			SataDriver() {};
			void init( const char * lock_name );

			/// @brief 获取端口命令槽；不检查命令槽是否忙状态；不需要配置命令表地址
			/// @param port 端口号
			/// @param head_index 命令槽号
			/// @return 命令槽的头部
			ata::sata::HbaCmdHeader *get_cmd_header( uint port, uint head_index );

			/// @brief 获取端口命令表
			/// @param port 端口号
			/// @param slot_index 命令槽号
			/// @return 命令表结构体指针
			ata::sata::HbaCmdTbl *get_cmd_table( uint port, uint slot_index );

			/// @brief 发布命令。不等待设备响应
			/// @param port 端口号
			/// @param cmd_slot 命令槽号
			void send_cmd( uint port, uint cmd_slot );

			/// @brief 询问命令槽是否空闲。设备完成命令之后命令槽便处于空闲状态
			/// @param port 端口号
			/// @param cmd_slot 命令槽
			/// @return 空闲则返回true
			bool request_cmdslot_idle( uint port, uint cmd_slot );

			/// @brief 清除端口中断
			/// @param port 端口
			/// @param intr 中断号枚举
			void clear_interrupt( uint port, ata::sata::HbaRegPortIs intr ) { assert( port < _port_num, "" ); _hba_port_reg[ port ]->is = intr; }

			/// @brief 获取可用端口数量
			/// @return 
			uint get_port_num() { return _port_num; }

			// void set_sector_size( uint sector_size ) { _logical_sector_size = sector_size; }

			/// @brief 获取逻辑扇区大小
			/// @return 逻辑扇区大小是访问硬盘的最小单位
			uint get_sector_size() { return _logical_sector_size; }

			/// @brief 询问端口是否正忙
			bool port_is_busy( uint port ) { return _hba_port_reg[ port ]->tfd & ata::sata::HbaRegPortTfd::hba_port_tfd_sts_bsy_m; }

		// 初始化前使用的函数
		public:
			/// @brief 设置HBA的内部寄存器基地址，应在初始化前完成设置
			/// @param base HBA内部寄存器基地址(V)，应该从PCI配置头中读出
			void set_hba_mem_reg( void *base ) { _hba_mem_reg = ( ata::sata::HbaMemReg * ) base; }

			/// @brief 设置端口的Command List基地址，应在初始化前完成设置
			/// @param port 端口号
			/// @param base 预先分配好给Command List空间的基地址(P)
			void set_port_clb( uint port, void *base ) { _port_cmd_lst_base[ port ] = ( ata::sata::HbaCmdList* ) base; }

			/// @brief 设置端口可用命令槽的数量，应在初始化前完成设置
			/// @details 一个命令槽会分配一个页面，一个端口最多32个命令槽
			/// @param port 端口号
			/// @param cmd_slot_cnt 命令槽
			void set_port_cmd_slot_cnt( uint port, uint8 cmd_slot_cnt ) { assert( cmd_slot_cnt < 32, "" ); _port_cmd_slot_num[ port ] = cmd_slot_cnt; }

			/// @brief 设置端口的Receive FIS基地址，应在初始化前完成设置
			/// @param port 端口号
			/// @param base 预先分配好给RFIS的空间基地址(P)
			void set_port_fb( uint port, void *base ) { _port_rec_fis_base[ port ] = ( ata::sata::HbaRevFis* ) base; }

			/// @brief 设置端口数量，应在初始化前完成设置
			/// @param num 端口数量，最大32个，通常不超过6个
			void set_port_num( uint num ) { assert( num < 32, "" ); _port_num = num; }

		// debug using 
		public:
			// 探测SATA端口状态
			void sata_probe();

			// 获取端口的中断状态字
			uint32 get_port_is( uint port ) { return _hba_port_reg[ port ]->is; }

			// 调试输出收到的D2H帧
			void debug_print_port_d2h_fis( uint i );
		};

		extern SataDriver k_sata_driver;
	} // namespace sata

} // namespace ata


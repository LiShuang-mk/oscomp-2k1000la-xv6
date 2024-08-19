//
// Created by Li Shuang ( pseudonym ) on 2024-08-17
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include <ata/ahci.hh>
#include <ata/sata.hh>
#include <block_device.hh>
#include <disk_partition_device.hh>
#include <hsai_global.hh>
#include <hsai_log.hh>
#include <kernel/klib/function.hh>
#include <mem/virtual_memory.hh>
#include <smp/spin_lock.hh>

namespace loongarch
{
	namespace qemu2k1000
	{
		class AhciDriverLs;

		constexpr uint ahci_port_dev_name_max_len = 8;
		constexpr uint ahci_port_default_cmd_slot = 0;

		class AhciPortDriverLs : public hsai::BlockDevice
		{
			friend AhciDriverLs;

		private:

			hsai::SpinLock _lock;

			char _dev_name[ahci_port_dev_name_max_len];

			int _port_id = 0;

			hsai::AhciPortReg *_regs	= nullptr;
			hsai::AhciCmdList *_cmd_lst = nullptr;
			hsai::AhciRevFis  *_rev_fis = nullptr;
			hsai::AhciCmdTbl  *_cmd_tbl = nullptr;

			uint _default_cmd_slot = 0;
			uint _block_size	   = 0;

			std::function<int( void )> _call_back;

			hsai::DiskPartitionDevice _disk_partition[4]; // MBR 硬盘只支持最多4个分区

			char _partition_name[4][8];

		public:

			virtual long get_block_size() override { return (long) _block_size; }
			virtual int	 read_blocks_sync( long start_block, long block_count,
										   hsai::BufferDescriptor *buf_list,
										   int					   buf_count ) override;
			virtual int	 read_blocks( long start_block, long block_count,
									  hsai::BufferDescriptor *buf_list, int buf_count ) override;
			virtual int	 write_blocks_sync( long start_block, long block_count,
											hsai::BufferDescriptor *buf_list,
											int						buf_count ) override;
			virtual int	 write_blocks( long start_block, long block_count,
									   hsai::BufferDescriptor *buf_list, int buf_count ) override;
			virtual int	 handle_intr() override;

			virtual bool read_ready() override
			{
				return !_cmd_slot_busy( _default_cmd_slot ) && !_task_busy();
			}
			virtual bool write_ready() override
			{
				return !_cmd_slot_busy( _default_cmd_slot ) && !_task_busy();
			}

		public:

			AhciPortDriverLs() = default;
			AhciPortDriverLs( const char *lock_name, int port_id, void *base_addr, void *cmd_list,
							  void *fis_base, void *cmd_table );

			void isu_cmd_identify( void *buffer, uint len,
								   std::function<int( void )> callback_handler );

			void isu_cmd_read_dma(
				u64 lba, u16 blk_cnt, uint prd_cnt,
				std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
				std::function<int( void )> callback_handler );

			void isu_cmd_write_dma(
				uint64 lba, uint64 blk_cnt, uint prd_cnt,
				std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
				std::function<int( void )> callback_handler );

		private:

			constexpr bool _cmd_slot_busy( uint cmd_slot )
			{
				return ( _regs->ci & ( 1U << cmd_slot ) ) != 0;
			}

			constexpr bool _task_busy()
			{
				return ( _regs->tfd & ( hsai::ahci_port_tfd_sts_bsy_m ) ) != 0;
			}

			void _issue_command_slot( uint slot ) { _regs->ci |= 0x1U << slot; }

			void _fill_command_head( hsai::AhciCmdHeader &head, u32 cmd_table_len )
			{
				head.prdtl = cmd_table_len;
				head.pmp   = 0;
				head.rsv   = 0;
				head.c	   = 0;
				head.b	   = 0;
				head.r	   = 0;
				head.p	   = 0;
				head.w	   = 0;
				head.a	   = 0;
				head.cfl   = 5;
				head.prdbc = 0;
				if ( head.ctba == 0 && head.ctbau == 0 )
				{
					u64 addr   = hsai::k_mem->to_dma( (u64) _cmd_tbl );
					head.ctba  = (u32) ( addr & 0xFFFF'FFFFUL );
					head.ctbau = (u32) ( addr >> 32 );
				}

				head.rsv_dword[0] = head.rsv_dword[1] = head.rsv_dword[2] = head.rsv_dword[3] = 0;
			}

			void _fill_command_fis( hsai::SataFisRegH2D &fis, u8 command, u64 lba, u16 sector_cnt,
									u16 features, u8 device )
			{
				fis.fis_type	 = hsai::sata_fis_reg_h2d;
				fis.pm_port		 = 0;
				fis.c			 = 1;
				fis.command		 = command;
				fis.features	 = (u8) ( features & 0xFF );
				fis.features_exp = (u8) ( features >> 8 );
				fis.device		 = device;
				fis.control		 = 0;

				fis.lba_low		 = (u8) ( ( lba >> 0 ) & 0xFF );
				fis.lba_mid		 = (u8) ( ( lba >> 8 ) & 0xFF );
				fis.lba_high	 = (u8) ( ( lba >> 16 ) & 0xFF );
				fis.lba_low_exp	 = (u8) ( ( lba >> 24 ) & 0xFF );
				fis.lba_mid_exp	 = (u8) ( ( lba >> 32 ) & 0xFF );
				fis.lba_high_exp = (u8) ( ( lba >> 40 ) & 0xFF );

				fis.sector_cnt	   = (u8) ( sector_cnt & 0xFF );
				fis.sector_cnt_exp = (u8) ( sector_cnt >> 8 );

				fis.rsv0 = 0;
				fis.rsv1 = 0;
				fis.rsv2 = 0;
			}

			int ahci_port_default_call_back( void )
			{
				hsai_warn( "%s : 引发了一个中断，但是没有配置中断回调函数", _dev_name );
				return -1;
			};

		public:

			void debug_print_cmd_head();

			void debug_print_cmd_table( uint cmd_slot );
		};

	} // namespace qemu2k1000

} // namespace loongarch

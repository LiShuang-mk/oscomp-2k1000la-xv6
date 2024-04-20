//
// Created by Li shuang ( pseudonym ) on 2024-04-14 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/sata/fis.hh"
#include "hal/sata/hba_cmd.hh"
#include "hal/sata/hba_port.hh"
#include "hal/ata/ata_cmd.hh"
#include "hal/qemu_ls2k.hh"
#include "fs/dev/ahci_controller.hh"
#include "fs/dev/sata_driver.hh"
#include "fs/fat/fat32.hh"
#include "mm/physical_memory_manager.hh"
#include "klib/common.hh"

namespace dev
{
	namespace ahci
	{
		AhciController k_ahci_ctl;

		void AhciController::init( const char *lock_name )
		{
			_lock.init( lock_name );
		}

		void AhciController::isu_cmd_identify( uint port, void *buffer, uint len )
		{
			assert( port < sata::k_sata_driver.get_port_num() );
			if ( len < 512 )
			{
				log__warn(
					"buffer size is not enough.\n"
					"identify command would not be issue" );
				return;
			}

			// 获取command table 
			ata::sata::HbaCmdTbl *cmd_tbl = sata::k_sata_driver.get_cmd_table( port, 0 );
			assert( ( uint64 ) cmd_tbl != 0x0UL );

			// 命令表使用的 FIS 为 H2D
			struct ata::sata::FisRegH2D *fis_h2d = ( struct ata::sata::FisRegH2D * ) cmd_tbl->cmd_fis;
			fis_h2d->fis_type = ata::sata::FisType::fis_reg_h2d;
			fis_h2d->pm_port = 0;		// 端口复用使用的值，这里写0就可以了，不涉及端口复用
			fis_h2d->c = 1;				// 表示这是一个主机发给设备的命令帧
			fis_h2d->command = ata::AtaCmd::cmd_identify_device;
			fis_h2d->features = fis_h2d->features_exp = 0;		// refer to ATA8-ACS, this field should be N/A ( or 0 ) when the command is 'indentify' 
			fis_h2d->device = 0;								// similar to above 
			fis_h2d->lba_low = fis_h2d->lba_low_exp = 0;
			fis_h2d->lba_mid = fis_h2d->lba_mid_exp = 0;
			fis_h2d->lba_high = fis_h2d->lba_high_exp = 0;
			fis_h2d->sector_cnt = fis_h2d->sector_cnt_exp = 0;
			fis_h2d->control = 0;

			// physical region address 
			void *pr = ( void * )
				( loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) buffer )
					| loongarch::qemuls2k::iodma_win_base );

			// 暂时直接引用 0 号命令槽
			struct ata::sata::HbaCmdHeader* head = sata::k_sata_driver.get_cmd_header( port, 0 );
			log_trace( "head address: %p", head );

			// 设置命令头 
			head->prdtl = 1;
			head->pmp = 0;
			head->c = 1;		// 传输结束清除忙状态
			head->b = 0;
			head->r = 0;
			head->p = 0;
			head->w = 0;
			head->a = 0;// 先清除上次的中断
			head->cfl = 5;
			head->prdbc = 0;	// should be set 0 before issue command 

			// 注意下面两句已经被废弃，命令表的空间分配被迁移至初始化过程中
			// 发送命令不再需要分配空间给命令表
			// head->ctba = ( uint32 ) loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) cmd_tbl );
			// head->ctbau = ( uint32 ) ( loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) cmd_tbl ) >> 32 );

			// 设置数据区 
			ata::sata::HbaPrd *prd0 = &cmd_tbl->prdt[ 0 ];
			prd0->dba = ( uint64 ) loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) pr );
			prd0->interrupt = 1;
			prd0->dbc = 512 - 1;

			// 这里的清中断应转移到中断处理函数中
			// sata::k_sata_driver.clear_interrupt( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m );

			// log_trace( "before issue cmd, port pss: %d", sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );

			_pr = pr;

			// 发布命令
			sata::k_sata_driver.send_cmd( port, 0 );
		}


		void AhciController::isu_cmd_read_dma( uint64 lba )
		{
			// command table address 
			struct ata::sata::HbaCmdTbl *cmd_tbl = ( struct ata::sata::HbaCmdTbl * ) mm::k_pmm.alloc_page();
			assert( ( uint64 ) cmd_tbl != 0x0UL );
			mm::k_pmm.clear_page( ( void* ) cmd_tbl );

			// 命令表使用的 FIS 类型是 H2D
			struct ata::sata::FisRegH2D *fis_h2d = ( struct ata::sata::FisRegH2D * ) cmd_tbl->cmd_fis;
			fis_h2d->fis_type = ata::sata::FisType::fis_reg_h2d;
			fis_h2d->command = ata::AtaCmd::cmd_read_dma;
			fis_h2d->lba_low = ( uint8 ) ( lba >> 0 );
			fis_h2d->lba_mid = ( uint8 ) ( lba >> 8 );
			fis_h2d->lba_high = ( uint8 ) ( lba >> 16 );
			fis_h2d->lba_low_exp = ( uint8 ) ( lba >> 24 );
			fis_h2d->lba_mid_exp = ( uint8 ) ( lba >> 32 );
			fis_h2d->lba_high_exp = ( uint8 ) ( lba >> 40 );
			fis_h2d->sector_cnt = 1;
			fis_h2d->sector_cnt_exp = 0;
			fis_h2d->c = 1;
			fis_h2d->device = 1 << 6;

			// physical region address 
			void *pr = mm::k_pmm.alloc_page();
			assert( ( uint64 ) pr != 0x0UL );
			mm::k_pmm.clear_page( pr );

			// 暂时直接引用指定的 0 号端口 0 号命令槽
			struct ata::sata::HbaCmdHeader* head = sata::k_sata_driver.get_cmd_header( 0, 0 );
			log_trace( "head address: %p", head );

			head->prdtl = 1;
			head->c = 1;
			head->cfl = 5;
			head->prdbc = 0;
			head->ctba = ( uint32 ) ( uint64 ) cmd_tbl;
			head->ctbau = ( uint32 ) ( ( uint64 ) cmd_tbl >> 32 );

			struct ata::sata::HbaPrd *prd0 = &cmd_tbl->prdt[ 0 ];
			prd0->dba = ( uint64 ) pr;
			prd0->interrupt = 1;
			prd0->dbc = mm::pg_size - 1;

			// 这里的清中断应转移到中断处理函数中
			// sata::k_sata_driver.clear_interrupt( 0, ata::sata::HbaRegPortIs::hba_port_is_dhrs_m );

			_pr = pr;
			_cmd_tbl = cmd_tbl;


			// log_trace( "before issue cmd, port pss: %d", sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );
			// 发布命令
			sata::k_sata_driver.send_cmd( 0, 0 );
			// while ( 1 );

			// int time_out = 1000;
			// // log_trace( "after issue cmd, port pss: %d", ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );
			// while ( !sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m )
			// 	&& time_out )
			// 	--time_out;
			// if ( time_out <= 0 )
			// {
			// 	log_error( "AHCI: 发送 Indentify 指令没有响应" );
			// 	mm::k_pmm.free_page( ( void* ) cmd_tbl );
			// 	mm::k_pmm.free_page( pr );
			// 	return;
			// }

			// time_out = 10000;
			// while ( ata::sata::k_sata_driver.request_cmd_finish( 0, 0 )
			// 	&& time_out )
			// 	// --time_out;
			// 	;
			// if ( time_out <= 0 )
			// {
			// 	log_error( "AHCI: HBA 接收数据超时" );
			// 	mm::k_pmm.free_page( ( void* ) cmd_tbl );
			// 	mm::k_pmm.free_page( pr );
			// 	return;
			// }

			// time_out = 10000;
			// while ( time_out )
			// 	--time_out;

			// log__info( "AHCI: read-dma 指令结束" );
			// log_trace( "打印扇区数据, 地址: %p", pr );

			// printf( "________________________________________________________________\n" );
			// uchar *rec_dat = ( uchar* ) pr;
			// for ( uint i = 0; i < 512; ++i )
			// {
			// 	if ( i % 0x10 == 0 )
			// 		printf( "%B\t", i );
			// 	printf( "%B ", rec_dat[ i ] );
			// 	if ( i % 0x10 == 0xF )
			// 		printf( "\n" );
			// }

			// mm::k_pmm.free_page( ( void* ) cmd_tbl );
			// mm::k_pmm.free_page( pr );
		}

		void AhciController::simple_intr_handle()
		{
			uint32 tmp;
			for ( int i = 0; i < 3; i++ )
			{
				tmp = sata::k_sata_driver.get_port_is( i );
				if ( tmp )
				{
					log_trace(
						"sata intr.\n"
						"port %d - is = %p",
						i, tmp
					);
					sata::k_sata_driver.debug_print_port_d2h_fis( i );
					sata::k_sata_driver.clear_interrupt( i, ( ata::sata::HbaRegPortIs ) tmp );
				}
			}

			if ( _pr == nullptr )
				return;

			log__info(
				"打印收到的数据\n"
				"\b\b\b\b________________________________\n"
			);

			printf( "  \t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
			uchar *p = ( uchar * ) _pr;
			for ( uint i = 0; i < 512; ++i )
			{
				if ( i % 0x10 == 0 )
					printf( "%B\t", i );
				printf( "%B ", p[ i ] );
				if ( i % 0x10 == 0xF )
					printf( "\n" );
			}

			if ( _is_idtf )
			{
				for ( int i = 0; i < 4; i++ )
				{
					p = ( uchar * ) _pr + 0X1BE + i * 16;
					log_trace(
						"partition %d:\n"
						"\tdrive attribute = %B\n"
						"\tCHS addr start  = %B%B%Bh\n"
						"\tpartition type  = %B\n"
						"\tCHS addr end    = %B%B%Bh\n"
						"\tLBA addr start  = %p\n"
						"\tnumber of sector= %d\n",
						i, *p,
						*( p + 3 ), *( p + 2 ), *( p + 1 ),
						*( p + 4 ),
						*( p + 7 ), *( p + 6 ), *( p + 5 ),
						*( uint32* ) ( p + 8 ),
						*( uint32* ) ( p + 0xc )
					);
				}

				_is_idtf = false;
			}
			else
			{
				struct fs::fat::Fat32Dbr* dbr = ( struct fs::fat::Fat32Dbr* ) _pr;
				if ( compare( dbr->ebpb.system_id, "FAT32 ", 6 ) == 0 )
				{
					log_trace(
						"扇区大小:              %d bytes\n"
						"簇大小:                %d sectors\n"
						"保留扇区数:            %d\n"
						"FAT 数量:              %d\n"
						"根目录条目数量:        %d\n"
						"硬盘介质类型:          %x\n"
						"一个磁道的扇区数:      %d\n"
						"磁头的数量:            %d\n"
						"隐藏扇区数量:          %d\n"
						"逻辑分区中的扇区数量:  %d\n",
						dbr->bpb.bytes_per_sector,
						dbr->bpb.sectors_per_cluster,
						dbr->bpb.reserved_sector_count,
						dbr->bpb.table_count,
						dbr->bpb.root_entry_count,
						dbr->bpb.media_type,
						dbr->bpb.sectors_per_track,
						dbr->bpb.head_side_count,
						dbr->bpb.hidden_sector_count,
						dbr->bpb.total_sectors_32
					);
				}
			}
		}
	}
// namespace ahci

} // namespace dev

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
			for ( auto &port_call_back_list : _call_back_function )
			{
				for ( auto &call_back : port_call_back_list )
				{
					call_back = [] ()->void
					{
						log__info( "<---- AHCI -- 默认中断回调 ---->" );
					};
				}
			}
		}

		void AhciController::isu_cmd_identify( uint port, void *buffer, uint len, std::function<void( void )> callback_handler )
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
			head->a = 0;
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

			// _pr = ( void* ) ( ( uint64 ) pr | loongarch::qemuls2k::dmwin::win_0 );

			// 设置中断回调函数
			if ( callback_handler != nullptr )
				_call_back_function[ port ][ 0 ] = callback_handler;

				// 发布命令
			sata::k_sata_driver.send_cmd( port, 0 );
		}


		void AhciController::isu_cmd_read_dma( uint port, uint64 lba, void *buffer, uint64 len, std::function<void( void )> callback_handler )
		{
			assert( port < sata::k_sata_driver.get_port_num() );
			if ( len < sata::k_sata_driver._logical_sector_size )
			{
				log__warn(
					"buffer size is not enough.\n"
					"read dma command would not be issue" );
				return;
			}
			if ( len % sata::k_sata_driver._logical_sector_size != 0 )
			{
				log__warn(
					"buffer size is not align to logical sector size\n"
					"read dma command would not be issue"
				);
				log_trace( "^^^^ logcial sector size : %d bytes ^^^^", sata::k_sata_driver._logical_sector_size );
				return;
			}
			if ( len > _1M * 4 )
			{
				log_error(
					"AHCI : 当前不支持超过4MiB的数据通过DMA读取\n"
					"       read DMA command would not be issue"
				);
			}

			// 获取command table 
			ata::sata::HbaCmdTbl *cmd_tbl = sata::k_sata_driver.get_cmd_table( port, 0 );
			assert( ( uint64 ) cmd_tbl != 0x0UL );

			// 命令表使用的 FIS 为 H2D
			struct ata::sata::FisRegH2D *fis_h2d = ( struct ata::sata::FisRegH2D * ) cmd_tbl->cmd_fis;
			fis_h2d->fis_type = ata::sata::FisType::fis_reg_h2d;
			fis_h2d->pm_port = 0;		// 端口复用使用的值，这里写0就可以了，不涉及端口复用
			fis_h2d->c = 1;				// 表示这是一个主机发给设备的uint64 lba 命令帧
			fis_h2d->command = ata::AtaCmd::cmd_read_dma;
			fis_h2d->features = fis_h2d->features_exp = 0;		// refer to ATA8-ACS, this field should be N/A ( or 0 ) when the command is 'indentify' 
			fis_h2d->device = 1 << 6;							// similar to above 
			fill_fis_h2d_lba( fis_h2d, lba );
			fis_h2d->sector_cnt = fis_h2d->sector_cnt_exp = len / sata::k_sata_driver._logical_sector_size;
			fis_h2d->control = 0;

			// physical region address 
			void *pr = ( void * )
				( loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) buffer )
					| loongarch::qemuls2k::iodma_win_base );

			// 暂时直接引用 0 号命令槽
			uint cmd_slot_num = 0;
			struct ata::sata::HbaCmdHeader* head = sata::k_sata_driver.get_cmd_header( port, cmd_slot_num );
			// log_trace( "head address: %p", head );

			// 设置命令头 
			head->prdtl = 1;
			head->pmp = 0;
			head->c = 1;		// 传输结束清除忙状态
			head->b = 0;
			head->r = 0;
			head->p = 0;
			head->w = 0;
			head->a = 0;
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
			prd0->dbc = len - 1;

			// 这里的清中断应转移到中断处理函数中
			// sata::k_sata_driver.clear_interrupt( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m );

			// log_trace( "before issue cmd, port pss: %d", sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );

			// _pr = ( void* ) ( ( uint64 ) pr | loongarch::qemuls2k::dmwin::win_0 );

			// 设置中断回调函数
			if ( callback_handler != nullptr )
				_call_back_function[ port ][ cmd_slot_num ] = callback_handler;

			// 发布命令
			sata::k_sata_driver.send_cmd( port, cmd_slot_num );
		}

		void AhciController::intr_handle()
		{
			uint32 tmp;
			for ( uint i = 0; i < sata::k_sata_driver.get_port_num(); i++ )
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

			_call_back_function[ 0 ][ 0 ]();
		}

		// void AhciController::simple_intr_handle()
		// {
		// 	uint32 tmp;
		// 	for ( uint i = 0; i < sata::k_sata_driver.get_port_num(); i++ )
		// 	{
		// 		tmp = sata::k_sata_driver.get_port_is( i );
		// 		if ( tmp )
		// 		{
		// 			log_trace(
		// 				"sata intr.\n"
		// 				"port %d - is = %p",
		// 				i, tmp
		// 			);
		// 			sata::k_sata_driver.debug_print_port_d2h_fis( i );
		// 			sata::k_sata_driver.clear_interrupt( i, ( ata::sata::HbaRegPortIs ) tmp );
		// 		}
		// 	}

		// 	return;

		// 	// log_trace(
		// 	// 	"扇区大小:              %d bytes\n"
		// 	// 	"簇大小:                %d sectors\n"
		// 	// 	"保留扇区数:            %d\n"
		// 	// 	"FAT 数量:              %d\n"
		// 	// 	"根目录条目数量:        %d\n"
		// 	// 	"硬盘介质类型:          %x\n"
		// 	// 	"一个磁道的扇区数:      %d\n"
		// 	// 	"磁头的数量:            %d\n"
		// 	// 	"隐藏扇区数量:          %d\n"
		// 	// 	"逻辑分区中的扇区数量:  %d\n",
		// 	// 	dbr->bpb.bytes_per_sector,
		// 	// 	dbr->bpb.sectors_per_cluster,
		// 	// 	dbr->bpb.reserved_sector_count,
		// 	// 	dbr->bpb.table_count,
		// 	// 	dbr->bpb.root_entry_count,
		// 	// 	dbr->bpb.media_type,
		// 	// 	dbr->bpb.sectors_per_track,
		// 	// 	dbr->bpb.head_side_count,
		// 	// 	dbr->bpb.hidden_sector_count,
		// 	// 	dbr->bpb.total_sectors_32
		// 	// );
		// }

// -------- private --------
		void AhciController::fill_fis_h2d_lba( ata::sata::FisRegH2D *fis, uint64 lba )
		{
			fis->lba_low = ( uint8 ) ( ( lba >> 0 ) & 0xFF );
			fis->lba_mid = ( uint8 ) ( ( lba >> 8 ) & 0xFF );
			fis->lba_high = ( uint8 ) ( ( lba >> 16 ) & 0xFF );
			fis->lba_low_exp = ( uint8 ) ( ( lba >> 24 ) & 0xFF );
			fis->lba_mid_exp = ( uint8 ) ( ( lba >> 32 ) & 0xFF );
			fis->lba_high_exp = ( uint8 ) ( ( lba >> 40 ) & 0xFF );
		}
	}
// namespace ahci

} // namespace dev

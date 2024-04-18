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
#include "hal/sata/sata_ls2k.hh"
#include "hal/ata/ata_cmd.hh"
#include "hal/qemu_ls2k.hh"
#include "fs/dev/ahci_controller.hh"
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


		void AhciController::isu_cmd_identify()
		{
			// command table address 
			struct ata::sata::HbaCmdTbl *cmd_tbl = ( struct ata::sata::HbaCmdTbl * ) mm::k_pmm.alloc_page();
			assert( ( uint64 ) cmd_tbl != 0x0UL );
			mm::k_pmm.clear_page( ( void* ) cmd_tbl );

			// 命令表使用的 FIS 为 H2D
			struct ata::sata::FisRegH2D *fis_h2d = ( struct ata::sata::FisRegH2D * ) cmd_tbl->cmd_fis;
			fis_h2d->fis_type = ata::sata::FisType::fis_reg_h2d;
			fis_h2d->command = ata::AtaCmd::cmd_indentify_device;
			fis_h2d->c = 1;				// write command register 
			fis_h2d->device = 0;		// master device 

			// physical region address 
			void *pr = mm::k_pmm.alloc_page();
			assert( ( uint64 ) pr != 0x0UL );
			mm::k_pmm.clear_page( pr );

			// 暂时直接引用指定的 0 号端口 0 号命令槽
			struct ata::sata::HbaCmdHeader* head = ata::sata::k_sata_driver.get_cmd_header( 0, 0 );
			log_trace( "head address: %p", head );

			// 设置命令头 
			head->prdtl = 1;
			head->c = 1;
			head->cfl = 5;
			head->prdbc = 0;
			head->ctba = ( uint32 ) loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) cmd_tbl );
			head->ctbau = ( uint32 ) ( loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) cmd_tbl ) >> 32 );

			// 设置数据区 
			struct ata::sata::HbaPrd *prd0 = &cmd_tbl->prdt[ 0 ];
			prd0->dba = ( uint64 ) loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) pr );
			prd0->interrupt = 1;
			prd0->dbc = mm::pg_size - 1;

			// 先清除上次的中断
			ata::sata::k_sata_driver.clear_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m );

			log_trace( "before issue cmd, port pss: %d", ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );

			_pr = pr;
			_cmd_tbl = cmd_tbl;

			// 发布命令
			ata::sata::k_sata_driver.start_send_cmd( 0, 0 );

			log__info( "debug测试, 进入loop等待中断" );
			while ( 1 )
				;

			int time_out = 1000;
			log_trace( "after issue cmd, port pss: %d", ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );
			while ( ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m )
				&& time_out )
				--time_out;
			if ( time_out <= 0 )
			{
				log_error( "AHCI: 发送 Indentify 指令没有响应" );
				mm::k_pmm.free_page( ( void* ) cmd_tbl );
				mm::k_pmm.free_page( pr );
				return;
			}

			time_out = 10000;
			while ( !ata::sata::k_sata_driver.request_cmd_finish( 0, 0 )
				&& time_out )
				// --time_out;
				;
			if ( time_out <= 0 )
			{
				log_error( "AHCI: HBA 接收数据超时" );
				mm::k_pmm.free_page( ( void* ) cmd_tbl );
				mm::k_pmm.free_page( pr );
				return;
			}

			time_out = 10000;
			while ( time_out )
				--time_out;


			ata::sata::k_sata_driver.debug_print_port_d2h_fis( 0 );

			log__info( "AHCI: Identify 指令结束" );
			log_trace( "打印硬盘识别信息, 地址: %p", pr );

			printf( "________________________________________________________________\n" );
			uchar *rec_dat = ( uchar* ) pr;
			for ( uint i = 0; i < 512; ++i )
			{
				if ( i % 0x10 == 0 )
					printf( "%B\t", i );
				printf( "%B ", rec_dat[ i ] );
				if ( i % 0x10 == 0xF )
					printf( "\n" );
			}

			mm::k_pmm.free_page( ( void* ) cmd_tbl );
			mm::k_pmm.free_page( pr );
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
			struct ata::sata::HbaCmdHeader* head = ata::sata::k_sata_driver.get_cmd_header( 0, 0 );
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

			// 先清除上次的中断
			ata::sata::k_sata_driver.clear_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_dhrs_m );

			_pr = pr;
			_cmd_tbl = cmd_tbl;

			log_trace( "before issue cmd, port pss: %d", ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );
			// 发布命令
			ata::sata::k_sata_driver.start_send_cmd( 0, 0 );
			while ( 1 );

			int time_out = 1000;
			log_trace( "after issue cmd, port pss: %d", ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );
			while ( !ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m )
				&& time_out )
				--time_out;
			if ( time_out <= 0 )
			{
				log_error( "AHCI: 发送 Indentify 指令没有响应" );
				mm::k_pmm.free_page( ( void* ) cmd_tbl );
				mm::k_pmm.free_page( pr );
				return;
			}

			time_out = 10000;
			while ( ata::sata::k_sata_driver.request_cmd_finish( 0, 0 )
				&& time_out )
				// --time_out;
				;
			if ( time_out <= 0 )
			{
				log_error( "AHCI: HBA 接收数据超时" );
				mm::k_pmm.free_page( ( void* ) cmd_tbl );
				mm::k_pmm.free_page( pr );
				return;
			}

			time_out = 10000;
			while ( time_out )
				--time_out;

			log__info( "AHCI: read-dma 指令结束" );
			log_trace( "打印扇区数据, 地址: %p", pr );

			printf( "________________________________________________________________\n" );
			uchar *rec_dat = ( uchar* ) pr;
			for ( uint i = 0; i < 512; ++i )
			{
				if ( i % 0x10 == 0 )
					printf( "%B\t", i );
				printf( "%B ", rec_dat[ i ] );
				if ( i % 0x10 == 0xF )
					printf( "\n" );
			}

			mm::k_pmm.free_page( ( void* ) cmd_tbl );
			mm::k_pmm.free_page( pr );
		}

		void AhciController::isu_cmd_write_dma( uint64 lba ){
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
			struct ata::sata::HbaCmdHeader* head = ata::sata::k_sata_driver.get_cmd_header( 0, 0 );
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

			// 先清除上次的中断
			ata::sata::k_sata_driver.clear_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_dhrs_m );

			_pr = pr;
			_cmd_tbl = cmd_tbl;

			log_trace( "before issue cmd, port pss: %d", ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );
			// 发布命令
			ata::sata::k_sata_driver.start_send_cmd( 0, 0 );
			while ( 1 );

			int time_out = 1000;
			log_trace( "after issue cmd, port pss: %d", ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m ) );
			while ( !ata::sata::k_sata_driver.request_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_pss_m )
				&& time_out )
				--time_out;
			if ( time_out <= 0 )
			{
				log_error( "AHCI: 发送 Indentify 指令没有响应" );
				mm::k_pmm.free_page( ( void* ) cmd_tbl );
				mm::k_pmm.free_page( pr );
				return;
			}

			time_out = 10000;
			while ( ata::sata::k_sata_driver.request_cmd_finish( 0, 0 )
				&& time_out )
				// --time_out;
				;
			if ( time_out <= 0 )
			{
				log_error( "AHCI: HBA 接收数据超时" );
				mm::k_pmm.free_page( ( void* ) cmd_tbl );
				mm::k_pmm.free_page( pr );
				return;
			}

			time_out = 10000;
			while ( time_out )
				--time_out;

			log__info( "AHCI: read-dma 指令结束" );
			log_trace( "打印扇区数据, 地址: %p", pr );

			printf( "________________________________________________________________\n" );
			uchar *rec_dat = ( uchar* ) pr;
			for ( uint i = 0; i < 512; ++i )
			{
				if ( i % 0x10 == 0 )
					printf( "%B\t", i );
				printf( "%B ", rec_dat[ i ] );
				if ( i % 0x10 == 0xF )
					printf( "\n" );
			}

			mm::k_pmm.free_page( ( void* ) cmd_tbl );
			mm::k_pmm.free_page( pr );
		}

		void AhciController::simple_read( uint64 lba )
		{
			ata::sata::k_sata_driver.clear_port_intr( 0, ata::sata::HbaRegPortIs::hba_port_is_dhrs_m );
			log__info( "AHCI 开始读取" );
			ata::sata::HbaPortReg *port_reg = ata::sata::k_sata_driver.debug_get_port_reg_base( 0 );
			log_trace( "调试 : port 0 tfd = %p", port_reg->tfd );

			// command table address 
			struct ata::sata::HbaCmdTbl *cmd_tbl = ( struct ata::sata::HbaCmdTbl * ) mm::k_pmm.alloc_page();
			assert( ( uint64 ) cmd_tbl != 0x0UL );
			mm::k_pmm.clear_page( ( void* ) cmd_tbl );

			struct ata::sata::FisRegH2D *fis_h2d = ( struct ata::sata::FisRegH2D * ) cmd_tbl->cmd_fis;
			fis_h2d->fis_type = ata::sata::FisType::fis_reg_h2d;

			// physical region address 
			void *pr = mm::k_pmm.alloc_page();
			assert( ( uint64 ) pr != 0x0UL );
			mm::k_pmm.clear_page( pr );

			// 使用 0 号命令槽
			struct ata::sata::HbaCmdHeader* head = ata::sata::k_sata_driver.get_cmd_header( 0, 0 );
			log_trace( "head address: %p", head );

			head->ctba = ( uint32 ) loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) cmd_tbl );
			head->ctbau = ( uint32 ) ( loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) cmd_tbl ) >> 32 );
			cmd_tbl->prdt[ 0 ].dba = loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) pr );
			fis_h2d->lba_low = ( lba >> 0x00 ) & 0xFFU;
			fis_h2d->lba_low_exp = ( lba >> 0x08 ) & 0xFFU;
			fis_h2d->lba_mid = ( lba >> 0x10 ) & 0xFFU;
			fis_h2d->lba_mid_exp = ( lba >> 0x18 ) & 0xFFU;
			fis_h2d->lba_high = ( lba >> 0x20 ) & 0xFFU;
			fis_h2d->lba_high_exp = ( lba >> 0x28 ) & 0xFFU;
			fis_h2d->command = ata::AtaCmd::cmd_read_dma;

			_cmd_tbl = cmd_tbl;
			_pr = pr;

			ata::sata::k_sata_driver.start_send_cmd( 0, 0 );

			log_trace( "调试 : port 0 tfd = %p", port_reg->tfd );
		}

		void AhciController::simple_intr_handle()
		{
			uint32 tmp;
			for ( int i = 0; i < 3; i++ )
			{
				tmp = ata::sata::k_sata_driver.get_port_is( i );
				if ( tmp )
				{
					log_trace(
						"sata intr.\n"
						"port %d - is = %p",
						i, tmp
					);
					ata::sata::k_sata_driver.debug_print_port_d2h_fis( i );
					ata::sata::k_sata_driver.clear_port_intr( i, ( ata::sata::HbaRegPortIs ) tmp );
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

			mm::k_pmm.free_page( ( void * ) _cmd_tbl );
			mm::k_pmm.free_page( ( void * ) _pr );
		}
	}
// namespace ahci

} // namespace dev

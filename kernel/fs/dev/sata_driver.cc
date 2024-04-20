//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/physical_memory_manager.hh"
#include "fs/dev/sata_driver.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/sata/hba_mem.hh"
#include "hal/sata/hba_cmd.hh"
#include "hal/sata/hba_fis.hh"
#include "hal/sata/fis.hh"

namespace dev
{
	namespace sata
	{
		SataDriver k_sata_driver;

		void SataDriver::init( const char *lock_name )
		{
			_lock.init( lock_name );

			// 设置端口的配置地址
			for ( uint i = 0; i < ata::sata::max_port_num; i++ )
				_hba_port_reg[ i ] = ( ata::sata::HbaPortReg * ) &_hba_mem_reg->ports[ i ];
			// log_trace( "SATA port 0 配置基地址: %p\n", &_hba_mem_reg->ports[ 0 ] );

			sata_probe();
			// debug_print_hba_mem();

			// 清中断
			// _hba_port_reg[ 0 ]->is = ( uint32 ) ~0x0U;
			// _hba_mem_reg->is = ( uint32 ) ~0x0U;

			for ( uint i = 0; i < _port_num; ++i )
			{
				// 配置 command list 地址
				log_trace( "set port %d clb : %p", i, _port_cmd_lst_base[ i ] );
				_hba_port_reg[ i ]->clb = ( ( uint32 ) ( uint64 ) _port_cmd_lst_base[ i ] );
				_hba_port_reg[ i ]->clbu = ( uint32 ) ( ( uint64 ) _port_cmd_lst_base[ i ] >> 32 );

				// 分配 command table 
				ata::sata::HbaCmdHeader *head;
				uint64 page;
				for ( uint j = 0; j < ata::sata::max_cmd_slot; j++ )
				{
					head = get_cmd_header( i, j );
					page = ( uint64 ) mm::k_pmm.alloc_page();
					page = loongarch::qemuls2k::virt_to_phy_address( page );
					head->ctba = ( uint32 ) page;
					head->ctbau = ( uint32 ) ( page >> 32 );
				}
				
				// 配置 receive FIS 地址 
				log_trace( "set port %d fb : %p", i, _port_rec_fis_base[ i ] );
				_hba_port_reg[ i ]->fb = ( ( uint32 ) ( uint64 ) _port_rec_fis_base[ i ] );
				_hba_port_reg[ i ]->fbu = ( uint32 ) ( ( uint64 ) _port_rec_fis_base[ i ] >> 32 );

				// 使能中断
				_hba_mem_reg->ghc |= ata::sata::HbaRegGhc::hba_ghc_ie_m;
				_hba_port_reg[ i ]->ie |= ata::sata::HbaRegPortIe::hba_port_ie_dhre_m;

				// 启动设备 
				_hba_port_reg[ i ]->cmd |= ata::sata::HbaRegPortCmd::hba_port_cmd_fre_m;
				_hba_port_reg[ i ]->cmd |= ata::sata::HbaRegPortCmd::hba_port_cmd_st_m;
			}
		}

		ata::sata::HbaCmdHeader* SataDriver::get_cmd_header( uint port, uint head_index )
		{
			assert( port < _port_num );
			assert( head_index < ata::sata::max_cmd_slot );

			// 使用非缓存窗口
			ata::sata::HbaCmdHeader _port_cmd_lst_base*head =
				( ata::sata::HbaCmdHeader* ) ( ( uint64 ) _port_cmd_lst_base[ port ]
					| loongarch::qemuls2k::dmwin::win_1 );
			head += head_index;
			return head;
		}

		ata::sata::HbaCmdTbl* SataDriver::get_cmd_table( uint port, uint slot_index )
		{
			assert( port < _port_num );
			assert( slot_index < ata::sata::max_cmd_slot );

			ata::sata::HbaCmdHeader *head = get_cmd_header( port, slot_index );
			uint64 addr = ( uint4 ) ( head->ctba );
			addr |= ( uint64 ) ( head->ctbau ) << 32;
			// 使用非缓存窗口
			addr |= loongarch::qemuls2k::dmwin::win_0;
			ata::sata::HbaCmdTbl *tbl = ( uint64 ) addr;
			return tbl;
		}

		void SataDriver::send_cmd( uint port, uint cmd_slot )
		{
			assert( port < _port_num );
			assert( cmd_slot < ata::sata::max_cmd_slot );
			_hba_port_reg[ port ]->ci = 0x1U << cmd_slot;
		}

		bool SataDriver::request_cmdslot_idle( uint port, uint cmd_slot )
		{
			assert( port < _port_num );
			assert( cmd_slot < ata::sata::max_cmd_slot );
			return !( _hba_port_reg[ port ]->ci & ( 0x1U << cmd_slot ) );
		}

		void SataDriver::sata_probe()
		{
			ata::sata::HbaPortReg *port_reg;
			for (uint i = 0; i < ata::sata::max_port_num; i++)
			{
				port_reg = &_hba_mem_reg->ports[ i ];
				log_trace(
					"[探测] port %d(%p), ssts %p\n"
					"signature : %p",
					i,
					port_reg,
					port_reg->ssts,
					port_reg->sig
				);
			}
			
		}

		void SataDriver::debug_print_port_d2h_fis( uint i )
		{
			assert( i < 32 );
			log_trace( "port[%d].is  = %x", i, _hba_port_reg[ i ]->is );
			if ( ( _hba_port_reg[ i ]->is & 0x1U ) )
			{
				ata::sata::HbaRevFis *rev_fis = ( ata::sata::HbaRevFis* ) ( ( uint64 ) _hba_port_reg[ i ]->fb | loongarch::qemuls2k::dmwin::win_0 );
				log_trace( "receive d2h fis => addr: %p", rev_fis );
				for ( int i = 0; i < 20; ++i )
					printf( "%B ", rev_fis->rfis[ i ] );
				printf( "\n" );
			}
		}
	} // namespace sata

} // namespace ata


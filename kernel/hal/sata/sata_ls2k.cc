//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/sata/sata_ls2k.hh"
#include "hal/sata/hba_mem.hh"
#include "hal/sata/hba_port.hh"
#include "hal/sata/hba_fis.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/pci/pci_cfg_header.hh"
#include "klib/common.hh"

namespace ata
{
	namespace sata
	{
		SataLs2k k_sata_driver;

		void SataLs2k::init( const char * lock_name, void *clb, void *fb )
		{
			SataDriver::init( lock_name, clb, fb );

			pci::PciCfgHeader *pciHead = ( pci::PciCfgHeader * ) loongarch::qemuls2k::PciCfgDevAddr::pci_cfg_sata;
			printf( "%x\n", pciHead->vendor_id );
			if ( pciHead->vendor_id != 0x0014 )
				log_panic( "pci head - vendor id is not 0x0014" );

			uint64 sata_mem_base = pciHead->base_address[ 0 ];
			sata_mem_base |= ( uint64 ) pciHead->base_address[ 1 ] << 32;
			sata_mem_base |= loongarch::qemuls2k::dmwin::win_1;
			log_trace( "SATA Memory Registers Base: %p\n", sata_mem_base );

			_hba_mem_reg = ( ata::sata::HbaMemReg * ) sata_mem_base;
			log_trace( "SATA CAP: %x", _hba_mem_reg->cap );
			log_trace( "SATA Ports Implemented: %x", _hba_mem_reg->pi );
			log_trace( "SATA AHCI Version: %x", _hba_mem_reg->vs );
			// printf( "\n", hba_mem_reg->cap );
			// printf( "SATA Ports Implemented: %x\n", hba_mem_reg->pi );
			// printf( "SATA AHCI Version: %x\n", hba_mem_reg->vs );

			_hba_vendor_reg = ( HbaMemVendorRegLs2k * ) _hba_mem_reg->vendor_reg;

			// _port_num = 1;
			// _hba_port_reg[ 0 ] = ( ata::sata::HbaPortReg * ) &( _hba_mem_reg->ports[ 0 ] );
			// _hba_port_reg[ 0 ]->sctl = ( _hba_mem_reg->cap & ( 0xF << 20 ) ) >> 20 << 4;
			// _hba_port_reg[ 0 ]->clb = ( uint32 ) ( ( uint64 ) clb );
			// _hba_port_reg[ 0 ]->clbu = ( uint32 ) ( ( uint64 ) clb >> 32 );
			// _hba_port_reg[ 0 ]->fb = ( uint32 ) ( ( uint64 ) fb );
			// _hba_port_reg[ 0 ]->fbu = ( uint32 ) ( ( uint64 ) fb >> 32 );
			// log_trace( "CLB = %p", clb );
			// log_trace( "FB  = %p", fb );
			// log_trace( "port[0].cmd = %x", _hba_port_reg[ 0 ]->cmd );
			// log_trace( "port[0].tfd = %x", _hba_port_reg[ 0 ]->tfd );
			// _hba_port_reg[ 0 ]->cmd |= ( uint32 ) ( 0x1U << 4 );
			// log_trace( "port[0].cmd = %x", _hba_port_reg[ 0 ]->cmd );
			// _hba_port_reg[ 0 ]->cmd |= 0x1U;
			// while ( ( ( _hba_port_reg[ 0 ]->cmd ) & ( 0x1U << 15 ) ) == 0 )
			// 	;
			// log_trace( "port[0].cmd = %x", _hba_port_reg[ 0 ]->cmd );

			// log_trace( "port[0].is  = %x", _hba_port_reg[ 0 ]->is );
			// if ( ( _hba_port_reg[ 0 ]->is & 0x1U ) )
			// {
			// 	HbaRevFis *rev_fis = ( HbaRevFis* ) ( ( uint64 ) fb | loongarch::qemuls2k::dmwin::win_0 );
			// 	log_trace( "receive d2h fis => addr: %p", rev_fis );
			// 	for ( int i = 0; i < 20; ++i )
			// 		printf( "%x ", rev_fis->rfis[ i ] );
			// 	printf( "\n" );
			// }
			int rc = ahci_host_init();
			if ( rc < 0 )
			{
				log_trace( "AHCI host init fail, return code = %d", rc );
				log_panic( "AHCI 初始化失败！" );
			}

			_print_ahci_info();

			uint i = 0;
			for ( uint j = 0x1U; i < _port_num; j <<= 1 )
			{
				if ( ( _port_map & j ) == 0 )
					continue;

				_start_port( i );
				++i;
			}
		}

		/// @brief ahci 主设备初始化
		/// @return 返回码，小于 0 则初始化失败
		int SataLs2k::ahci_host_init()
		{
			int time_out;

			// 准备使能 staggered spin-up 
			uint32 cap_save;
			cap_save = _hba_mem_reg->cap;
			cap_save |= HbaRegCap::hba_cap_sss_m;

			// hba 控制器复位
			uint32 tmp;
			tmp = _hba_mem_reg->ghc;
			log_trace( "sata 控制器复位前, ghc = %x", tmp );
			if ( ( tmp & HbaRegGhc::hba_ghc_hr_m ) == 0 )
			{
				log__info( "sata 控制器复位" );
				_hba_mem_reg->ghc = tmp | HbaRegGhc::hba_ghc_hr_m;
			}
			log_trace( "sata 控制器复位后, ghc = %x", tmp );

			time_out = 1000;
			while ( ( _hba_mem_reg->ghc & HbaRegGhc::hba_ghc_hr_m ) && time_out )
				--time_out;
			if ( time_out <= 0 )
			{
				log__warn( "sata controller reset fail" );
				return -1;
			}

			/* Set timer 1ms @ 100MHz*/
			_hba_vendor_reg->timer1ms = 100000000 / 1000;

			/* Setup OOBR */
			_hba_vendor_reg->oobr = HbaLs2kOobr::hba_oobr_we;
			_hba_vendor_reg->oobr = 0x02060b14;

			// 使能 AHCI 
			_hba_mem_reg->ghc = HbaRegGhc::hba_ghc_ae_m;

			log_trace( "before enable stag, cap = %x", _hba_mem_reg->cap );
			// 使能 staggered spin-up 
			_hba_mem_reg->cap = cap_save;
			log_trace( "after enable stag, cap = %x", _hba_mem_reg->cap );

			// 获得端口数量
			_port_num = ( cap_save & HbaRegCap::hba_cap_np_m ) + 1;

			// 初始化 PI 寄存器以获得可用的端口数
			_hba_mem_reg->pi = ( uint32 ) ( ( 0x1UL << _port_num ) - 1 );

			// 获得可用端口
			_cap_cache = _hba_mem_reg->cap;
			_port_map = _hba_mem_reg->pi;
			_port_num = ( _cap_cache & HbaRegCap::hba_cap_np_m ) + 1;

			log_trace( "CAP: %x, port map: %x, port num: %x", _hba_mem_reg->cap, _port_map, _port_num );

			//
			// 这里暂时将端口硬设置为1个 
			//
			_port_num = 1;

			// 接下来对每个端口初始化
			HbaPortReg *port_reg;
			uint i = 0;
			for ( uint j = 0x1U; i < _port_num && j != 0; j <<= 1 )
			{
				if ( ( _port_map & j ) == 0 )
					continue;

				port_reg = &_hba_mem_reg->ports[ i ];

				// 确保控制器端口处于 idle 状态
				tmp = port_reg->cmd;
				if ( tmp & (
					HbaRegPortCmd::hba_port_cmd_cr_m |
					HbaRegPortCmd::hba_port_cmd_fr_m |
					HbaRegPortCmd::hba_port_cmd_fre_m |
					HbaRegPortCmd::hba_port_cmd_st_m ) )
				{
					// 通过清除 st 位，并等待 cr 返回 0
					// 可以使端口进入 idle 状态
					tmp &= ~HbaRegPortCmd::hba_port_cmd_st_m;
					port_reg->cmd = tmp;

					time_out = 1000;
					while ( ( port_reg->cmd & HbaRegPortCmd::hba_port_cmd_cr_m ) && time_out )
						--time_out;
					if ( time_out <= 0 )
					{
						log_error( "sata 端口初始化失败" );
						printf( "port %d init fail", i );
						return -2;
					}
				}

				// Spin-up 设备
				port_reg->cmd |= HbaRegPortCmd::hba_port_cmd_sud_m;
				time_out = 1000;
				while ( !( port_reg->cmd | HbaRegPortCmd::hba_port_cmd_sud_m ) && time_out )
					--time_out;
				if ( time_out <= 0 )
				{
					log_error( "sata spin-up 未能完成！" );
					return -3;
				}

				// 等待检测设备
				time_out = 1000;
				while ( time_out )
				{
					tmp = port_reg->ssts;
					tmp &= HbaRegPortSsts::hba_port_ssts_det_m;
					if ( tmp == 0x3 || tmp == 0x1 ) // device detected 
					{
						log_trace( "device detection: %x", tmp );
						break;
					}
					--time_out;
				}
				if ( time_out <= 0 )
				{
					log_trace( "port %d 检测设备超时", i );
					log_error( "sata 未能检测到设备！" );
					return -4;
				}

				// 等待 COMINIT 状态设置 SERR 寄存器的 26 位（DIAG.X）
				time_out = 1000;
				while ( !( port_reg->serr & HbaRegPortSerr::hba_port_ssts_diag_x_m ) && time_out )
					--time_out;
				if ( time_out <= 0 )
				{
					log_trace( "port %d SERR = %p", i, port_reg->serr );
					log_trace( "port %d IS = %p", i, port_reg->is );
					log_error( "DIAG.X 位没有被置位！" );
					// return -5;
					log__warn( "等待 DIAG.X 置位超时" );
				}

				// 通过向 SERR 寄存器写来复位
				tmp = port_reg->serr;
				log_trace( "port %d serr = %p", i, tmp );
				port_reg->serr = tmp;

				// 应答当前端口的中断请求
				tmp = _hba_mem_reg->is;
				if ( tmp )
					_hba_mem_reg->is = tmp;

				// 设置中断掩码 
				_hba_mem_reg->is = 0x1U << i;
				port_reg->ie =
					HbaRegPortIe::hba_port_ie_tfee_m |
					HbaRegPortIe::hba_port_ie_hbfe_m |
					HbaRegPortIe::hba_port_ie_hbde_m |
					HbaRegPortIe::hba_port_ie_ife_m |
					HbaRegPortIe::hba_port_ie_prce_m |
					HbaRegPortIe::hba_port_ie_pce_m |
					HbaRegPortIe::hba_port_ie_dpe_m |
					HbaRegPortIe::hba_port_ie_ufe_m |
					HbaRegPortIe::hba_port_ie_sdbe_m |
					HbaRegPortIe::hba_port_ie_dse_m |
					HbaRegPortIe::hba_port_ie_pse_m |
					HbaRegPortIe::hba_port_ie_dhre_m;

				tmp = port_reg->ssts;
				log_trace( "port %d status = %p", i, tmp );

				++i;
			}

			tmp = _hba_mem_reg->ghc;
			log_trace( "sata GHC = %p", tmp );
			_hba_mem_reg->ghc = tmp | HbaRegGhc::hba_ghc_ie_m;
			tmp = _hba_mem_reg->ghc;
			log_trace( "stat GHC = %p", tmp );

			return 0;
		}
		/// @brief 打印 ahci 信息
		void SataLs2k::_print_ahci_info()
		{

			uint32 speed = ( _cap_cache & HbaRegCap::hba_cap_iss_m ) >> HbaRegCap::hba_cap_iss_s;
			const char *speed_s;
			if ( speed == 1 )
			{
				speed_s = "1.5";
			}
			else if ( speed == 2 )
			{
				speed_s = "3";
			}
			else
			{
				speed_s = "?";
			}

			const char *scc_s = "SATA";
			uint32 version = _hba_mem_reg->vs;
			printf( "\n_____________________________________________\n" );
			log_trace(
				"AHCI 版本:    %d.%d%d \n"
				"命令槽数量:   %d \n"
				"端口数量:     %d \n"
				"端口支持速率: %s Gbps \n"
				"端口映射:     %p \n"
				"模式:         %s \n",
				( version >> 16 ) & 0xff,
				( version >> 8 ) & 0xff,
				( version >> 0 ) & 0xff,
				( ( _cap_cache >> hba_cap_ncs_s ) & hba_cap_ncs_m ) + 1,
				( ( _cap_cache >> hba_cap_np_s ) & hba_cap_np_m ) + 1,
				speed_s,
				_port_map,
				scc_s );

			printf( "\n_____________________________________________\n" );
			log_trace( "flags: \n"
				"\t%s%s%s%s%s%s\n"
				"\t%s%s%s%s%s%s%s\n",
				_cap_cache & hba_cap_s64a_m ? "64bit " : "",
				_cap_cache & hba_cap_hncq_m ? "ncq " : "",
				_cap_cache & hba_cap_ssntf_m ? "sntf " : "",
				_cap_cache & hba_cap_smps_m ? "mps " : "",
				_cap_cache & hba_cap_sss_m ? "stag " : "",
				_cap_cache & hba_cap_salp_m ? "pm " : "",
				_cap_cache & hba_cap_sal_m ? "led " : "",
				_cap_cache & hba_cap_sclo_m ? "clo " : "",
				_cap_cache & hba_cap_snzo_m ? "nz " : "",
				_cap_cache & hba_cap_sam_m ? "only " : "",
				_cap_cache & hba_cap_spm_m ? "pmp " : "",
				_cap_cache & hba_cap_pmd_m ? "pio " : "",
				_cap_cache & hba_cap_ssc_m ? "slum " : "",
				_cap_cache & hba_cap_psc_m ? "part " : "" );

			log_trace( "vendor 版本 = %x\n", _hba_vendor_reg->versionr );
		}

		void SataLs2k::_start_port( uint i )
		{
			assert( _port_cmd_lst_base[ i ] != 0x0UL );
			assert( _port_rec_fis_base[ i ] != 0x0UL );

			uint32 tmp;

			HbaPortReg *port_reg = &_hba_mem_reg->ports[ i ];

			tmp = port_reg->ssts;
			log_trace(
				"enter start port : %d\n"
				"          status : %p"
				, i, tmp );

			if ( ( tmp & HbaRegPortSsts::hba_port_ssts_det_m ) != 0x03U )
			{
				log_trace( "没有连接到 port %d", i );
				log_error( "启动端口失败" );
				return;
			}

			( ( void ) 0 );
		}

		void SataLs2k::debug_print_cmd_lst_base()
		{
			printf( "________________________________\n" );
			printf( "HBA port command list space:\n" );
			for ( uint i = 0; i < max_port_num; ++i )
			{
				printf( "port %d -> %p\n", i, _port_cmd_lst_base[ i ] );
			}
			printf( "________________________________\n" );
		}
		void SataLs2k::debug_print_rec_fis_base()
		{
			printf( "________________________________\n" );
			printf( "HBA port receive FIS space:\n" );
			for ( uint i = 0; i < max_port_num; ++i )
			{
				printf( "port %d -> %p\n", i, _port_rec_fis_base[ i ] );
			}
			printf( "________________________________\n" );
		}
	} // namespace sata

} // namespace ata

//
// Created by Li Shuang ( pseudonym ) on 2024-07-18 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "ata/ahci_driver.hh"
#include "memory_interface.hh"
#include "hsai_global.hh"
#include "device_manager.hh"
#include "mem/virtual_memory.hh"
#include "hsai_log.hh"

namespace hsai
{
	AhciDriver::AhciDriver( const char * lock_name, void * base_addr )
		: AhciDriver()
	{
		_lock.init( lock_name );
		_regs = ( AhciMemReg * ) base_addr;

		// 关闭中断

		_regs->generic.ghc &= ~ahci_ghc_ie_m;

		// 重启设备

		_regs->generic.ghc |= ahci_ghc_hr_m;
		while ( _regs->generic.ghc & ahci_ghc_hr_m );

		// 使能 AHCI

		_regs->generic.ghc |= ahci_ghc_ae_m;

		// 获取端口数量

		// ulong pn =
		// 	( ( _regs->generic.cap & ahci_cap_np_m ) >> ahci_cap_np_s ) + 1;

		// 获取端口位图

		ulong bpm = _regs->generic.pi;

		// 清中断

		_regs->generic.is = ( u32 ) -1;

		// 申请 Received FIS 空间

		AhciRevFis * rev_fis_space = ( AhciRevFis * ) alloc_pages(
			page_round_up( sizeof( AhciRevFis ) * ahci_max_port_num ) / page_size
		);
		rev_fis_space = ( AhciRevFis * ) k_mem->to_io( ( ulong ) rev_fis_space );

		// 申请 Command List 空间

		AhciCmdList * cmd_ls_space = ( AhciCmdList * ) alloc_pages(
			page_round_up( sizeof( AhciCmdList ) * ahci_max_port_num ) / page_size
		);
		cmd_ls_space = ( AhciCmdList * ) k_mem->to_io( ( ulong ) cmd_ls_space );

		// 逐个检测端口并初始化

		int i = 0;
		u32 tmp;
		for ( u32 l = 1; l != 0; l <<= 1, i++ )
		{
			// 未被硬件实现的端口
			if ( ( l & bpm ) == 0 )
				continue;

			// 未建立物理链接的端口
			if ( ( ( _regs->ports[ i ].ssts & ahci_port_ssts_det_m ) >> ahci_port_ssts_det_s ) != 0x3 )
				continue;

			// 只初始化 ATA 设备（不包含ATAPI）
			tmp = _regs->ports[ i ].sig;
			if ( tmp != sata_sig_ata )
				continue;
			hsai_trace( "port\t%d sig : %x", i, tmp );

			// 初始化端口

			new ( &_port_drivers[ i ] ) AhciPortDriver(
				"AHCI port",
				i,
				&_regs->ports[ i ],
				&cmd_ls_space[ i ],
				&rev_fis_space[ i ]
			);

			++_port_num;
			_port_bitmap |= l;
		}

		// 打开中断

		_regs->generic.ghc |= ahci_ghc_ie_m;

		// 注册到 HSAI
		k_devm.register_device( this, "AHCI driver" );
	}

	int AhciDriver::handle_intr()
	{
		int i = 0, res = 0;
		u32 tmp = _regs->generic.is;
		for ( u32 l = 1; l != 0; l <<= 1, i++ )
		{
			if ( ( l & _port_bitmap ) == 0 ) continue;
			if ( l & tmp )
			{
				int rc = _port_drivers[ i ].handle_intr();
				if ( rc < 0 )
					return rc;
				res++;
			}
		}
		_regs->generic.is = tmp;
		return res;
	}

	void AhciDriver::identify_device()
	{
		word * id_data = ( word * ) alloc_pages( 1 );
		bool cmd_finish;
		auto call_back = [ & ] () -> int
		{
			cmd_finish = true;
			return 0;
		};
		int i = 0;
		for ( u32 l = 1; l != 0; l <<= 1, i++ )
		{
			if ( ( l & _port_bitmap ) == 0 ) continue;
			cmd_finish = false;
			_port_drivers[ i ].isu_cmd_identify( ( void* ) id_data, page_size, call_back );
			while ( cmd_finish == false );
			_analyze_identify_data( i, id_data );
		}
	}

///////////////////////////////// private helper function /////////////////////////////////

	int AhciDriver::_analyze_identify_data( uint port_id, word * id_data )
	{
		if ( port_id > ahci_max_port_num )
		{
			hsai_error( "AHCI : invalid port id" );
			return -100;
		}
		word w106 = id_data[ 106 ];
		if ( ( w106 & ( 1U << 14 ) ) == 0
			|| ( w106 & ( 1U << 15 ) ) != 0 )
		{
			hsai_error( "identify AHCI port %d not valid", port_id );
			return -1;
		}
		if ( w106 & ( 1U << 12 ) )	// 逻辑扇区大小超过 256 words
		{
			word w117 = id_data[ 117 ], w118 = id_data[ 118 ];
			ulong logc_size = ( ulong ) ( w117 ) +( ( ulong ) w118 << 16 ) /*words*/;
			logc_size *= 2;		/*bytes*/
			_port_drivers[ port_id ]._block_size = logc_size;
		}
		else						// 逻辑扇区大小恰为 256 words
		{
			_port_drivers[ port_id ]._block_size = 512;
		}
		hsai_trace( "AHCI port %d block size is %d <Bytes>", port_id, _port_drivers[ port_id ]._block_size );
		return 0;
	}

} // namespace hsai

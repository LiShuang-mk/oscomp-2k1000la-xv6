//
// Created by Li Shuang ( pseudonym ) on 2024-07-18 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "ata/ahci_port_driver.hh"
#include "ata/ata.hh"
#include "memory_interface.hh"
#include "hsai_global.hh"
#include "device_manager.hh"
#include "mem/virtual_memory.hh"
#include "hsai_log.hh"

namespace hsai
{
	int AhciPortDriver::read_blocks_sync( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count )
	{
		if ( buf_count <= 0 )
		{
			hsai_warn( "不合法的缓冲区数量(%d)", buf_count );
			return -1;
		}
		bool finish = false;
		isu_cmd_read_dma( start_block, block_count, buf_count,
			/*set pr*/[ & ] ( uint i, u64 &addr, u32 &size ) -> void
		{
			if ( i >= ( uint ) buf_count ) return;
			addr = ( u64 ) buf_list[ i ].buf_addr;
			size = ( u32 ) buf_list[ i ].buf_size;
		},
			/*call back*/[ & ] () -> int
		{
			finish = true;
			return 0;
		} );
		while ( !finish );
		return 0;
	}

	int AhciPortDriver::read_blocks( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count )
	{
		hsai_panic( "not implement" );
		while ( 1 );
	}

	int AhciPortDriver::write_blocks_sync( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count )
	{
		if ( buf_count <= 0 )
		{
			hsai_warn( "不合法的缓冲区数量(%d)", buf_count );
			return -1;
		}
		bool finish = false;
		isu_cmd_write_dma( start_block, block_count, buf_count,
			/*set pr*/[ & ] ( uint i, u64 &addr, u32 &size ) -> void
		{
			if ( i >= ( uint ) buf_count ) return;
			addr = ( u64 ) buf_list[ i ].buf_addr;
			size = ( u32 ) buf_list[ i ].buf_size;
		},
			/*call back*/[ & ] () -> int
		{
			finish = true;
			return 0;
		} );
		while ( !finish );
		return 0;
	}

	int AhciPortDriver::write_blocks( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count )
	{
		hsai_panic( "not implement" );
		while ( 1 );
	}

	int AhciPortDriver::handle_intr()
	{
		u32 tmp = _regs->is;
		// hsai_trace( "AHCI port %d intr %p", _port_id, tmp );
		if ( tmp & ahci_port_is_dhrs_m )
		{
			_regs->is = tmp;
			return _call_back();
		}
		else
		{
			hsai_error( "AHCI : unknown intr (is=%p)", tmp );
			_regs->is = tmp;
			return -100;
		}
	}

	AhciPortDriver::AhciPortDriver(
		const char * lock_name,
		int port_id,
		void * base_addr,
		void * cmd_list,
		void * fis_base )
	{
		_regs = ( AhciPortReg * ) base_addr;
		_cmd_ls = ( AhciCmdList * ) cmd_list;
		_rev_fis = ( AhciRevFis * ) fis_base;
		_port_id = port_id;
		_call_back = std::bind( &AhciPortDriver::_default_callback, this );

		char * pname = ( char * ) _dev_name;
		for ( int i = 0; i < ( int ) sizeof _dev_name; i++ )
			pname[ i ] = ahci_port_driver_name_template[ i ];

		// 分配 Command Table 空间，这里简化设计，只使用一个只占一页大小的 Table
		// 这个 Table 分配给 0号命令槽，此后发送命令都只使用 0号命令槽

		AhciCmdTbl * cmd_tbl = ( AhciCmdTbl * ) alloc_pages( 1 );
		hsai_assert( ( ( ulong ) cmd_tbl % 128UL ) == 0, "AHCI 规定 command table 地址需要 128 字节对齐" );
		cmd_tbl = ( AhciCmdTbl * ) k_mem->to_io( ( ulong ) cmd_tbl );
		_cmd_tbl = ( AhciCmdTbl * ) cmd_tbl;

		// 先关闭端口以初始化

		_regs->cmd &= ~ahci_port_cmd_fre_m;
		while ( _regs->cmd & ahci_port_cmd_fr_m )
			;
		_regs->cmd &= ~ahci_port_cmd_st_m;
		while ( _regs->cmd & ahci_port_cmd_cr_m )
			;

		// 配置 command list 地址

		_regs->clb = ( u32 ) ( u64 ) k_mem->to_dma( ( ulong ) _cmd_ls );
		_regs->clbu = ( u32 ) ( ( u64 ) k_mem->to_dma( ( ulong ) _cmd_ls ) >> 32 );
		// hsai_trace( "AHCI trace CLB %p", *( u64 * ) &_regs->clb );

		// 配置 received FIS 地址

		_regs->fb = ( u32 ) ( u64 ) k_mem->to_dma( ( ulong ) _rev_fis );
		_regs->fbu = ( u32 ) ( ( u64 ) k_mem->to_dma( ( ulong ) _rev_fis ) >> 32 );
		// hsai_trace( "AHCI trace FB  %p", *( u64 * ) &_regs->fb );

		// 清除中断

		_regs->is = ( u32 ) -1;

		// 使能中断

		_regs->ie |=
			ahci_port_ie_ufe_m | ahci_port_ie_sdbe_m | ahci_port_ie_dse_m |
			ahci_port_ie_pse_m | ahci_port_ie_dhre_m;

		// 启动端口

		_regs->cmd |= ahci_port_cmd_fre_m;
		while ( !( _regs->cmd & ahci_port_cmd_fr_m ) );
		_regs->cmd |= ahci_port_cmd_st_m;
		while ( !( _regs->cmd & ahci_port_cmd_cr_m ) );

		// 注册到 HSAI

		if ( ahci_port_change_name_number( ( char * ) _dev_name, _port_id ) < 0 )
		{
			hsai_error(
				"unknown AHCI port driver init.\n"
				"the port number is too big." );
			return;
		}
		k_devm.register_block_device( this, _dev_name );
	}

	/****************************************************
	 * ！！！此处一定注意避坑！！！
	 * 在协议规范里面定义的数据结构，会包含大量 reserve
	 * 字段，这些字段是保留的，通常情况下置为0，否则可能
	 * 发生未知的例外情况
	 * 例如，中断可能无法正常产生。当然，这也有可能和具体
	 * 实现有关，至少在 loongarch qemu 2k1000 中是无法产生
	 * 中断的。
	 ****************************************************/

	void AhciPortDriver::isu_cmd_identify( void *buffer, uint len, std::function<int( void )> callback_handler )
	{
		if ( len < 512 )
		{
			hsai_warn(
				"buffer size is not enough.\n"
				"identify command would not be issue" );
			return;
		}

		// 配置命令头

		int cmd_slot = _default_cmd_slot;	// 仅使用 0 号命令槽

		AhciCmdHeader& head = _cmd_ls->headers[ cmd_slot ];

		head.prdtl = 1;
		head.pmp = 0;
		head.c = 1;			// 传输结束清除忙状态
		head.b = 0;
		head.r = 0;
		head.p = 0;
		head.w = 0;
		head.a = 0;
		head.cfl = 5;		// identify 命令长 5 dwords
		head.prdbc = 0;		// 硬件用于记录已传输的字节数，软件应复位为0
		head.ctba = ( u32 ) ( u64 ) k_mem->to_dma( ( u64 ) _cmd_tbl );					// 配置命令表地址
		head.ctbau = ( u32 ) ( ( u64 ) k_mem->to_dma( ( u64 ) _cmd_tbl ) >> 32 );		// 命令表由硬件通过DMA读取
		// hsai_trace( "AHCI port %d cmd table %p", _port_id, *( u64 * ) &head.ctba );

		// 配置 FIS

		SataFisRegH2D * fis_h2d = ( SataFisRegH2D * ) _cmd_tbl->cmd_fis;
		fis_h2d->fis_type = sata_fis_reg_h2d;
		fis_h2d->pm_port = 0;									// 端口复用使用的值，这里写0就可以了，不涉及端口复用
		fis_h2d->c = 1;											// 表示这是一个主机发给设备的命令帧
		fis_h2d->command = ata_cmd_identify_device;				// 配置为 identify 命令
		fis_h2d->features = fis_h2d->features_exp = 0;			// refer to ATA8-ACS, this field should be N/A ( or 0 ) when the command is 'indentify' 
		fis_h2d->device = 0;									// similar to above 
		fis_h2d->lba_low = fis_h2d->lba_low_exp = 0;
		fis_h2d->lba_mid = fis_h2d->lba_mid_exp = 0;
		fis_h2d->lba_high = fis_h2d->lba_high_exp = 0;
		fis_h2d->sector_cnt = fis_h2d->sector_cnt_exp = 0;
		fis_h2d->control = 0;

		// 设置数据区

		void *pr = ( void * ) hsai::k_mem->to_dma( ( ulong ) buffer );
		AhciPrd *prd0 = &_cmd_tbl->prdt[ 0 ];
		prd0->dba = ( u64 ) pr;
		prd0->interrupt = 1;
		prd0->dbc = len - 1;
		// hsai_trace( "AHCI port %d dba %p", _port_id, prd0->dba );

		// 设置中断回调函数

		if ( callback_handler != nullptr )
			_call_back = callback_handler;
		else
			_call_back = std::bind( &AhciPortDriver::_default_callback, this );

		// 发布命令

		_wait_while_busy( cmd_slot );
		_issue_command_slot( cmd_slot );
	}

	void AhciPortDriver::isu_cmd_read_dma(
		uint64 lba,
		uint64 blk_cnt,
		uint prd_cnt,
		std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
		std::function<int( void )> callback_handler )
	{
		// 配置命令头

		int cmd_slot = _default_cmd_slot;	// 仅使用 0 号命令槽

		AhciCmdHeader& head = _cmd_ls->headers[ cmd_slot ];

		head.prdtl = prd_cnt;
		head.pmp = 0;
		head.c = 1;			// 传输结束清除忙状态
		head.b = 0;
		head.r = 0;
		head.p = 0;
		head.w = 0;
		head.a = 0;
		head.cfl = 5;		// 命令类帧长 5 dwords
		head.prdbc = 0;		// 硬件用于记录已传输的字节数，软件应复位为0
		head.ctba = ( u32 ) ( u64 ) k_mem->to_dma( ( u64 ) _cmd_tbl );					// 配置命令表地址
		head.ctbau = ( u32 ) ( ( u64 ) k_mem->to_dma( ( u64 ) _cmd_tbl ) >> 32 );		// 命令表由硬件通过DMA读取
		// hsai_trace( "AHCI port %d cmd table %p", _port_id, *( u64 * ) &head.ctba );

		// 配置 FIS

		SataFisRegH2D * fis_h2d = ( SataFisRegH2D * ) _cmd_tbl->cmd_fis;
		fis_h2d->fis_type = sata_fis_reg_h2d;
		fis_h2d->pm_port = 0;									// 端口复用使用的值，这里写0就可以了，不涉及端口复用
		fis_h2d->c = 1;											// 表示这是一个主机发给设备的命令帧
		fis_h2d->command = ata_cmd_read_dma;					// 配置为 read(DMA) 命令
		fis_h2d->features = fis_h2d->features_exp = 0;			// refer to ATA8-ACS, this field should be N/A ( or 0 ) when the command is 'read dma' 
		fis_h2d->device = 1 << 6;									// refer to ATA8-ACS, bit 6 of this field should be set when the command is 'read dma' 
		_fill_fis_h2d_lba( fis_h2d, lba );
		fis_h2d->sector_cnt = ( u32 ) ( blk_cnt >> 0 );
		fis_h2d->sector_cnt_exp = ( u32 ) ( blk_cnt >> 32 );
		fis_h2d->control = 0;

		// 设置数据区

		AhciPrd *prd;
		uint64 dba;
		uint32 dbc;
		for ( uint i = 0; i < prd_cnt; i++ )
		{
			prd = &_cmd_tbl->prdt[ i ];
			dba = dbc = 0;
			set_prd_handler( i, dba, dbc );
			if ( dba == 0 || dbc == 0 )
			{
				hsai_error( "AHCI : 无效的DBA或DBC" );
				return;
			}
			if ( dbc > _1M * 4 )
			{
				hsai_warn( "AHCI : PR长度超过4MiB, read DMA命令将不会被发送" );
				return;
			}
			prd->dba = hsai::k_mem->to_dma( dba );
			prd->dbc = dbc - 1;
			prd->interrupt = 1;
		}

		// 设置中断回调函数

		if ( callback_handler != nullptr )
			_call_back = callback_handler;
		else
			_call_back = std::bind( &AhciPortDriver::_default_callback, this );

		// 发布命令

		_wait_while_busy( cmd_slot );
		_issue_command_slot( cmd_slot );
	}

	void AhciPortDriver::isu_cmd_write_dma(
		uint64 lba,
		uint64 blk_cnt,
		uint prd_cnt,
		std::function<void( uint prd_i, uint64&pr_base, uint32&pr_size )> set_prd_handler,
		std::function<int( void )> callback_handler )
	{
		// 配置命令头

		int cmd_slot = _default_cmd_slot;	// 仅使用 0 号命令槽

		AhciCmdHeader& head = _cmd_ls->headers[ cmd_slot ];

		head.prdtl = prd_cnt;
		head.pmp = 0;
		head.c = 1;			// 传输结束清除忙状态
		head.b = 0;
		head.r = 0;
		head.p = 0;
		head.w = 1;		// 本命令是一个写命令（数据方向从host到device）
		head.a = 0;
		head.cfl = 5;		// 命令类帧长 5 dwords
		head.prdbc = 0;		// 硬件用于记录已传输的字节数，软件应复位为0
		head.ctba = ( u32 ) ( u64 ) k_mem->to_dma( ( u64 ) _cmd_tbl );					// 配置命令表地址
		head.ctbau = ( u32 ) ( ( u64 ) k_mem->to_dma( ( u64 ) _cmd_tbl ) >> 32 );		// 命令表由硬件通过DMA读取

		// 配置 FIS

		SataFisRegH2D * fis_h2d = ( SataFisRegH2D * ) _cmd_tbl->cmd_fis;
		fis_h2d->fis_type = sata_fis_reg_h2d;
		fis_h2d->pm_port = 0;									// 端口复用使用的值，这里写0就可以了，不涉及端口复用
		fis_h2d->c = 1;											// 表示这是一个主机发给设备的命令帧
		fis_h2d->command = ata_cmd_write_dma;					// 配置为 read(DMA) 命令
		fis_h2d->features = fis_h2d->features_exp = 0;			// refer to ATA8-ACS, this field should be N/A ( or 0 ) when the command is 'read dma' 
		fis_h2d->device = 1 << 6;									// refer to ATA8-ACS, bit 6 of this field should be set when the command is 'read dma' 
		_fill_fis_h2d_lba( fis_h2d, lba );
		fis_h2d->sector_cnt = ( u32 ) ( blk_cnt >> 0 );
		fis_h2d->sector_cnt_exp = ( u32 ) ( blk_cnt >> 32 );
		fis_h2d->control = 0;

		// 设置数据区

		AhciPrd *prd;
		uint64 dba;
		uint32 dbc;
		for ( uint i = 0; i < prd_cnt; i++ )
		{
			prd = &_cmd_tbl->prdt[ i ];
			dba = dbc = 0;
			set_prd_handler( i, dba, dbc );
			if ( dba == 0 || dbc == 0 )
			{
				hsai_error( "AHCI : 无效的DBA或DBC" );
				return;
			}
			if ( dbc > _1M * 4 )
			{
				hsai_warn( "AHCI : PR长度超过4MiB, read DMA命令将不会被发送" );
				return;
			}
			prd->dba = hsai::k_mem->to_dma( dba );
			prd->dbc = dbc - 1;
			prd->interrupt = 1;
		}

		// 设置中断回调函数

		if ( callback_handler != nullptr )
			_call_back = callback_handler;
		else
			_call_back = std::bind( &AhciPortDriver::_default_callback, this );

		// 发布命令

		_wait_while_busy( cmd_slot );
		_issue_command_slot( cmd_slot );
	}

///////////////////////// private helper function ///////////////////////////////////////

	int AhciPortDriver::_default_callback()
	{
		hsai_warn( "%s : 引发了一个中断，但是没有配置中断回调函数", _dev_name );
		return -1;
	}

	void AhciPortDriver::_issue_command_slot( uint i )
	{
		hsai_assert( i < ahci_max_cmd_slot, "命令槽id号太大" );
		_regs->ci |= 0x1U << i;
	}

// >>>> debug

	void AhciPortDriver::debug_print_register()
	{
		hsai_printf( "ci %p\ntfd %p\n", _regs->ci, _regs->tfd );
	}

} // namespace hsai

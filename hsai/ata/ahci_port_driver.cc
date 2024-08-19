//
// Created by Li Shuang ( pseudonym ) on 2024-07-18
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "ata/ahci_port_driver.hh"

#include "ata/ata.hh"
#include "device_manager.hh"
#include "hsai_global.hh"
#include "hsai_log.hh"
#include "mem/virtual_memory.hh"
#include "memory_interface.hh"

namespace hsai
{
	int AhciPortDriver::read_blocks_sync( long start_block, long block_count,
										  BufferDescriptor *buf_list, int buf_count )
	{
		if ( buf_count <= 0 )
		{
			hsai_warn( "不合法的缓冲区数量(%d)", buf_count );
			return -1;
		}
		bool finish = false;
		isu_cmd_read_dma(
			start_block, block_count, buf_count,
			/*set pr*/
			[&]( uint i, u64 &addr, u32 &size ) -> void
			{
				if ( i >= (uint) buf_count ) return;
				addr = (u64) buf_list[i].buf_addr;
				size = (u32) buf_list[i].buf_size;
			},
			/*call back*/
			[&]() -> int
			{
				finish = true;
				// hsai_info( "ahci port read handler" );
				return 0;
			} );

		debug_print_cmd_head();
		debug_print_cmd_table( 0 );
		while ( !finish );

		hsai_trace( "print AHCI port regs" );
		volatile u32 *preg = (volatile u32 *) _regs;
		for ( int i = 0; i < 18; i++ )
		{
			hsai_printf( "%_08x ", preg[i] );
			if ( i % 4 == 3 ) hsai_printf( "\n" );
		}
		while ( _cmd_slot_busy( _default_cmd_slot ) )
		{
			for ( long i = 100'000'000; i >= 0; --i );

			hsai_trace( "print AHCI port regs" );
			volatile u32 *preg = (volatile u32 *) _regs;
			for ( int i = 0; i < 18; i++ )
			{
				hsai_printf( "%_08x ", preg[i] );
				if ( i % 4 == 3 ) hsai_printf( "\n" );
			}
		}


		return 0;
	}

	int AhciPortDriver::read_blocks( long start_block, long block_count, BufferDescriptor *buf_list,
									 int buf_count )
	{
		hsai_panic( "not implement" );
		while ( 1 );
	}

	int AhciPortDriver::write_blocks_sync( long start_block, long block_count,
										   BufferDescriptor *buf_list, int buf_count )
	{
		if ( buf_count <= 0 )
		{
			hsai_warn( "不合法的缓冲区数量(%d)", buf_count );
			return -1;
		}
		bool finish = false;
		isu_cmd_write_dma(
			start_block, block_count, buf_count,
			/*set pr*/
			[&]( uint i, u64 &addr, u32 &size ) -> void
			{
				if ( i >= (uint) buf_count ) return;
				addr = (u64) buf_list[i].buf_addr;
				size = (u32) buf_list[i].buf_size;
			},
			/*call back*/
			[&]() -> int
			{
				finish = true;
				return 0;
			} );
		while ( !finish );
		return 0;
	}

	int AhciPortDriver::write_blocks( long start_block, long block_count,
									  BufferDescriptor *buf_list, int buf_count )
	{
		hsai_panic( "not implement" );
		while ( 1 );
	}

	int AhciPortDriver::handle_intr()
	{
		u32 tmp = _regs->is;
		hsai_trace( "AHCI port %d intr IS=%#_010x SERR=%#_010x", _port_id, tmp, _regs->serr );
		int rc;

		{
			u8 *p = (u8 *) _rev_fis->rfis;
			hsai_trace( "print port %d FIS-D2H data", _port_id );
			hsai_printf(
				BLUE_COLOR_PRINT
				"buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
			for ( uint i = 0; i < sizeof _rev_fis->rfis; ++i )
			{
				if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
				hsai_printf( "%B ", p[i] );
				if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
			}
			hsai_printf( "\n" );
		}

		if ( tmp & ahci_port_is_dhrs_m )
		{
			rc		   = _call_back();
			_regs->is  = ahci_port_is_dhrs_m;
			tmp		  &= ~ahci_port_is_dhrs_m;
		}

		if ( tmp & ahci_port_is_pss_m )
		{
			rc		   = _call_back();
			_regs->is  = ahci_port_is_pss_m;
			tmp		  &= ~ahci_port_is_pss_m;
		}

		if ( tmp )
		{
			hsai_error( "AHCI : unknown intr (is=%p)", tmp );

			hsai_error( "AHCI port %d regs:", _port_id );
			u32 *preg = (u32 *) _regs;
			for ( int i = 0; i < 18; i++ )
			{
				hsai_printf( "%_08x ", preg[i] );
				if ( i % 4 == 3 ) hsai_printf( "\n" );
			}
			hsai_printf( "\n" );

			u8 *p = (u8 *) _rev_fis->rfis;
			hsai_trace( "print port %d FIS-D2H data", _port_id );
			hsai_printf(
				BLUE_COLOR_PRINT
				"buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
			for ( uint i = 0; i < sizeof _rev_fis->rfis; ++i )
			{
				if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
				hsai_printf( "%B ", p[i] );
				if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
			}
			hsai_printf( "\n" );

			_regs->is = tmp;

			// if ( _regs->serr ) _regs->serr = _regs->serr;

			rc = -100;
		}

		return rc;
	}

	AhciPortDriver::AhciPortDriver( const char *lock_name, int port_id, void *base_addr,
									void *cmd_list, void *fis_base )
	{
		_regs	   = (AhciPortReg *) base_addr;
		_cmd_ls	   = (AhciCmdList *) cmd_list;
		_rev_fis   = (AhciRevFis *) fis_base;
		_port_id   = port_id;
		_call_back = std::bind( &AhciPortDriver::_default_callback, this );

		char *pname = (char *) _dev_name;
		for ( int i = 0; i < (int) sizeof _dev_name; i++ )
			pname[i] = ahci_port_driver_name_template[i];

		// 分配 Command Table 空间，这里简化设计，只使用一个只占一页大小的 Table
		// 这个 Table 分配给 0号命令槽，此后发送命令都只使用 0号命令槽

		int			cmd_tbl_pg_cnt = 1;
		AhciCmdTbl *cmd_tbl		   = (AhciCmdTbl *) alloc_pages( cmd_tbl_pg_cnt );
		hsai_assert( ( (ulong) cmd_tbl % 128UL ) == 0,
					 "AHCI 规定 command table 地址需要 128 字节对齐" );
		cmd_tbl	 = (AhciCmdTbl *) k_mem->to_io( (ulong) cmd_tbl );
		_cmd_tbl = (AhciCmdTbl *) cmd_tbl;

		{
			u8	*pct = (u8 *) _cmd_tbl;
			u64 *p	 = (u64 *) pct;
			long cnt = cmd_tbl_pg_cnt * page_size / sizeof( u64 );
			for ( long i = 0; i < cnt; ++i ) p[i] = 0;
		}

		// debug print
		debug_print_cmd_head();
		debug_print_cmd_table( 0 );

		// 先关闭端口以初始化

		if ( _regs->cmd & ( ahci_port_cmd_st_m | ahci_port_cmd_cr_m | ahci_port_cmd_fre_m |
							ahci_port_cmd_fr_m ) )
		{
			if ( ( _regs->cmd & ahci_port_cmd_st_m ) != 0 )
			{
				_regs->cmd &= ~ahci_port_cmd_st_m;
				while ( ( _regs->cmd & ahci_port_cmd_cr_m ) != 0 );
			}

			if ( _regs->cmd & ( ahci_port_cmd_fre_m ) )
			{
				_regs->cmd &= ~ahci_port_cmd_fre_m;
				while ( ( _regs->cmd & ahci_port_cmd_fr_m ) != 0 );
			}
		}

		if ( _regs->cmd & ( ahci_port_cmd_cr_m | ahci_port_cmd_fr_m ) )
		{
			hsai_panic( "\e[5m" "端口 %d 关闭失败!", port_id );
		}

		// {
		// 	_regs->sctl = 0x1;
		// 	for ( int ii = 10'000'000; ii >= 0; --ii );
		// 	_regs->sctl = 0x0;
		// 	while ( ( ( _regs->ssts & ahci_port_ssts_det_m ) >> ahci_port_ssts_det_s ) != 0x3 )
		// 		if ( ( ( _regs->ssts & ahci_port_ssts_det_m ) >> ahci_port_ssts_det_s ) == 0x1 )
		// 			break;
		// 	_regs->serr = _regs->serr;
		// }

		// _regs->sctl = 0x1 << 4;

		hsai_trace( "Port %d cmd %#_010x", port_id, _regs->cmd );

		// 配置 command list 地址

		if ( _regs->clb == 0 && _regs->clbu == 0 )
		{
			_regs->clb	= (u32) (u64) k_mem->to_vir( (ulong) _cmd_ls );
			_regs->clbu = (u32) ( (u64) k_mem->to_vir( (ulong) _cmd_ls ) >> 32 );
		}
		else
		{
			_regs->clb	= (u32) (u64) k_mem->to_vir( (ulong) _cmd_ls );
			_regs->clbu = (u32) ( (u64) k_mem->to_vir( (ulong) _cmd_ls ) >> 32 );

			// _regs->clbu = (u32) ( (u64) k_mem->to_vir( 0 ) >> 32 );

			// _regs->clb	= 128 * _1M + 0 * page_size;
			// _regs->clbu = 0x9000'0000;

			_cmd_ls = (AhciCmdList *) k_mem->to_io( ( _regs->clb + ( (u64) _regs->clbu << 32 ) ) );
		}
		hsai_trace( "AHCI port %d trace CLB %p", port_id, _cmd_ls );

		// 配置 received FIS 地址

		if ( _regs->fb == 0 && _regs->fbu == 0 )
		{
			_regs->fb  = (u32) (u64) k_mem->to_vir( (ulong) _rev_fis );
			_regs->fbu = (u32) ( (u64) k_mem->to_vir( (ulong) _rev_fis ) >> 32 );
		}
		else
		{
			_regs->fb	= (u32) (u64) k_mem->to_vir( (ulong) _rev_fis );
			_regs->fbu = (u32) ( (u64) k_mem->to_vir( (ulong) _rev_fis ) >> 32 );

			// _regs->fbu = (u32) ( (u64) k_mem->to_vir( 0 ) >> 32 );

			// _regs->fb  = 128 * _1M + 1 * page_size;
			// _regs->fbu = 0x9000'0000;

			_rev_fis = (AhciRevFis *) k_mem->to_io( _regs->fb + ( (u64) _regs->fbu << 32 ) );
		}
		hsai_trace( "AHCI port %d trace FB  %p", port_id, _rev_fis );

		{
			// auto &head = _cmd_ls->headers[0];
			// u64	  ctb  = head.ctba + ( (u64) head.ctbau << 32 );
			// if ( ctb != 0 ) { _cmd_tbl = (AhciCmdTbl *) k_mem->to_io( ctb ); }

			// _cmd_tbl = (AhciCmdTbl *) ( ( 0x9UL << 60 ) + 128 * _1M + 2 * page_size );
		}

		// 打开接受FIS使能

		_regs->cmd |= ahci_port_cmd_fre_m;
		while ( ( _regs->cmd & ahci_port_cmd_fr_m ) == 0 );

		// 清除中断

		// _regs->is = _regs->is;

		// 清除error位

		_regs->serr = _regs->serr;

		// 使能中断

		_regs->ie |= ahci_port_ie_ufe_m | ahci_port_ie_sdbe_m | ahci_port_ie_dse_m |
					 ahci_port_ie_pse_m | ahci_port_ie_dhre_m;
		// _regs->ie |= -1;

		// 启动端口

		_regs->cmd |= ahci_port_cmd_st_m;
		while ( ( _regs->cmd & ahci_port_cmd_cr_m ) == 0 );

		// 激活端口

		_regs->cmd |= ahci_port_cmd_icc_active_m;
		while ( ( _regs->cmd & ahci_port_cmd_icc_m ) != 0 );

		// 等待端口空闲

		while ( ( _regs->tfd & ahci_port_tfd_sts_bsy_m ) != 0 );

		// 注册到 HSAI

		if ( ahci_port_change_name_number( (char *) _dev_name, _port_id ) < 0 )
		{
			hsai_error( "unknown AHCI port driver init.\n" "the port number is too big." );
			return;
		}
		for ( auto &pname : _partition_name )
		{
			for ( ulong i = 0; i < sizeof _dev_name; ++i ) pname[i] = _dev_name[i];
			pname[3] = 'p';
			pname[4] = '?';
			pname[5] = 0;
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

	void AhciPortDriver::isu_cmd_identify( void *buffer, uint len,
										   std::function<int( void )> callback_handler )
	{
		if ( len < 512 )
		{
			hsai_warn( "buffer size is not enough.\n" "identify command would not be issue" );
			return;
		}

		// 配置命令头

		int cmd_slot = _default_cmd_slot; // 仅使用 0 号命令槽

		AhciCmdHeader &head = _cmd_ls->headers[cmd_slot];

		head.prdtl = 1;
		head.pmp   = 0;
		head.c	   = 0; // 传输结束清除忙状态
		head.b	   = 0;
		head.r	   = 0;
		head.p	   = 0;
		head.w	   = 0;
		head.a	   = 0;
		head.cfl   = 5; // identify 命令长 5 dwords
		head.prdbc = 0; // 硬件用于记录已传输的字节数，软件应复位为0
		head.ctba  = (u32) (u64) k_mem->to_dma( (u64) _cmd_tbl ); // 配置命令表地址
		head.ctbau =
			(u32) ( (u64) k_mem->to_dma( (u64) _cmd_tbl ) >> 32 ); // 命令表由硬件通过DMA读取
		// hsai_trace( "AHCI port %d cmd table %p", _port_id, *( u64 * ) &head.ctba );

		// 配置 FIS

		SataFisRegH2D *fis_h2d = (SataFisRegH2D *) _cmd_tbl->cmd_fis;
		fis_h2d->fis_type	   = sata_fis_reg_h2d;
		fis_h2d->pm_port = 0; // 端口复用使用的值，这里写0就可以了，不涉及端口复用
		fis_h2d->c		  = 1; // 表示这是一个主机发给设备的命令帧
		fis_h2d->command  = ata_cmd_identify_device; // 配置为 identify 命令
		fis_h2d->features = fis_h2d->features_exp =
			0; // refer to ATA8-ACS, this field should be N/A ( or 0 ) when the command is
			   // 'indentify'
		fis_h2d->device	 = 0; // similar to above
		fis_h2d->lba_low = fis_h2d->lba_low_exp = 0;
		fis_h2d->lba_mid = fis_h2d->lba_mid_exp = 0;
		fis_h2d->lba_high = fis_h2d->lba_high_exp = 0;
		fis_h2d->sector_cnt = fis_h2d->sector_cnt_exp = 0;
		fis_h2d->control							  = 0;

		// 设置数据区

		void	*pr		= (void *) hsai::k_mem->to_dma( (ulong) buffer );
		AhciPrd *prd0	= &_cmd_tbl->prdt[0];
		prd0->dba		= (u64) pr;
		prd0->interrupt = 0;
		prd0->dbc		= len - 1;
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
		uint64 lba, uint64 blk_cnt, uint prd_cnt,
		std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
		std::function<int( void )>											callback_handler )
	{
		// 配置命令头

		int cmd_slot = _default_cmd_slot; // 仅使用 0 号命令槽

		AhciCmdHeader &head = _cmd_ls->headers[cmd_slot];

		head.prdtl = prd_cnt;
		head.pmp   = 0;
		head.rsv   = 0;
		head.c	   = 0; // 传输结束清除忙状态
		head.b	   = 0;
		head.r	   = 0;
		head.p	   = 0;
		head.w	   = 0;
		head.a	   = 0;
		head.cfl   = 5; // 命令类帧长 5 dwords
		head.prdbc = 0; // 硬件用于记录已传输的字节数，软件应复位为0
		head.ctba  = (u32) (u64) k_mem->to_vir( (u64) _cmd_tbl ); // 配置命令表地址
		head.ctbau =
			(u32) ( (u64) k_mem->to_vir( (u64) _cmd_tbl ) >> 32 ); // 命令表由硬件通过DMA读取

		head.rsv_dword[0] = head.rsv_dword[1] = head.rsv_dword[2] = head.rsv_dword[3] = 0;

		{
			// debug print
			hsai_trace( "print read cmd header (%p)", &head );
			u8	*p	  = (u8 *) &head;
			u32 *preg = (u32 *) p;
			for ( int i = 0; i < 8; i++ ) { hsai_printf( "%08x  ", preg[i] ); }
			hsai_printf( "\n" );
		}

		// 配置 FIS

		SataFisRegH2D *fis_h2d = (SataFisRegH2D *) _cmd_tbl->cmd_fis;
		fis_h2d->fis_type	   = sata_fis_reg_h2d;
		fis_h2d->pm_port = 0; // 端口复用使用的值，这里写0就可以了，不涉及端口复用
		fis_h2d->c			  = 1; // 表示这是一个主机发给设备的命令帧
		fis_h2d->command	  = ata_cmd_read_dma; // 配置为 read(DMA) 命令
		fis_h2d->features	  = 0;
		fis_h2d->features_exp = 0; // refer to ATA8-ACS, this field should be N/A ( or 0 ) when the
								   // command is 'read dma'
		fis_h2d->device = 1 << 6;  // refer to ATA8-ACS, bit 6 of this field should be set when the
								   // command is 'read dma'
		_fill_fis_h2d_lba( fis_h2d, lba );
		fis_h2d->sector_cnt		= (u8) ( ( blk_cnt >> 0 ) & 0xFF );
		fis_h2d->sector_cnt_exp = (u8) ( ( blk_cnt >> 8 ) & 0xFF );
		fis_h2d->control		= 0;

		fis_h2d->rsv0 = 0;
		fis_h2d->rsv1 = 0;
		fis_h2d->rsv2 = 0;


		{
			// debug print
			hsai_trace( "print read cmd FIS (%p)", fis_h2d );
			u8	*p	  = (u8 *) fis_h2d;
			u32 *preg = (u32 *) p;
			for ( int i = 0; i < 5; i++ ) { hsai_printf( "%08x  ", preg[i] ); }
			hsai_printf( "\n" );
		}

		// 设置数据区

		AhciPrd *prd;
		uint64	 dba;
		uint32	 dbc;
		for ( uint i = 0; i < prd_cnt; i++ )
		{
			prd = &_cmd_tbl->prdt[i];
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
			prd->dba	   = hsai::k_mem->to_vir( dba );
			prd->dbc	   = dbc - 1;
			prd->interrupt = 0;
			prd->resv1	   = 0;
			prd->resv2	   = 0;
		}

		// debug_print_cmd_table( 0 );

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
		uint64 lba, uint64 blk_cnt, uint prd_cnt,
		std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
		std::function<int( void )>											callback_handler )
	{
		// 配置命令头

		int cmd_slot = _default_cmd_slot; // 仅使用 0 号命令槽

		AhciCmdHeader &head = _cmd_ls->headers[cmd_slot];

		head.prdtl = prd_cnt;
		head.pmp   = 0;
		head.c	   = 1; // 传输结束清除忙状态
		head.b	   = 0;
		head.r	   = 0;
		head.p	   = 0;
		head.w	   = 1; // 本命令是一个写命令（数据方向从host到device）
		head.a	   = 0;
		head.cfl   = 5; // 命令类帧长 5 dwords
		head.prdbc = 0; // 硬件用于记录已传输的字节数，软件应复位为0
		head.ctba  = (u32) (u64) k_mem->to_dma( (u64) _cmd_tbl ); // 配置命令表地址
		head.ctbau =
			(u32) ( (u64) k_mem->to_dma( (u64) _cmd_tbl ) >> 32 ); // 命令表由硬件通过DMA读取

		// 配置 FIS

		SataFisRegH2D *fis_h2d = (SataFisRegH2D *) _cmd_tbl->cmd_fis;
		fis_h2d->fis_type	   = sata_fis_reg_h2d;
		fis_h2d->pm_port = 0; // 端口复用使用的值，这里写0就可以了，不涉及端口复用
		fis_h2d->c		  = 1;				   // 表示这是一个主机发给设备的命令帧
		fis_h2d->command  = ata_cmd_write_dma; // 配置为 read(DMA) 命令
		fis_h2d->features = fis_h2d->features_exp =
			0; // refer to ATA8-ACS, this field should be N/A ( or 0 ) when the command is 'read
			   // dma'
		fis_h2d->device = 1 << 6; // refer to ATA8-ACS, bit 6 of this field should be set when the
								  // command is 'read dma'
		_fill_fis_h2d_lba( fis_h2d, lba );
		fis_h2d->sector_cnt		= (u32) ( blk_cnt >> 0 );
		fis_h2d->sector_cnt_exp = (u32) ( blk_cnt >> 32 );
		fis_h2d->control		= 0;

		// 设置数据区

		AhciPrd *prd;
		uint64	 dba;
		uint32	 dbc;
		for ( uint i = 0; i < prd_cnt; i++ )
		{
			prd = &_cmd_tbl->prdt[i];
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
			prd->dba	   = hsai::k_mem->to_dma( dba );
			prd->dbc	   = dbc - 1;
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

	void AhciPortDriver::debug_print_cmd_head()
	{
		volatile u32		 *preg;
		volatile AhciCmdList *cl =
			(volatile AhciCmdList *) k_mem->to_io( _regs->clb + ( (ulong) _regs->clbu << 32 ) );
		hsai_trace( "AHCI port %d debug print cmd headers", _port_id );
		for ( long i = 0; auto &head : cl->headers )
		{
			u8 *p = (u8 *) &head;
			preg  = (volatile u32 *) p;
			hsai_printf( "head %d :  %#_010x  %#_010x  %#_010x  %#_010x\n", i, preg[0], preg[1],
						 preg[2], preg[3] );
			++i;
		}
	}

	void AhciPortDriver::debug_print_cmd_table( uint cmd_slot )
	{
		volatile AhciCmdList *cl =
			(volatile AhciCmdList *) k_mem->to_io( _regs->clb + ( (ulong) _regs->clbu << 32 ) );
		volatile AhciCmdTbl *tbl = (volatile AhciCmdTbl *) k_mem->to_io(
			cl->headers[cmd_slot].ctba + ( (ulong) cl->headers[cmd_slot].ctbau << 32 ) );
		auto		 &head = cl->headers[cmd_slot];
		volatile u32 *pword;
		volatile u8	 *p;


		p	  = (volatile u8 *) tbl->cmd_fis;
		pword = (volatile u32 *) p;
		hsai_trace( "AHCI port %d cmd-slot %d cmd-FIS (%p):", _port_id, cmd_slot, p );
		u32 cmd_len = head.cfl; // dword (4 bytes)

		hsai_printf( ">>  " );
		for ( u32 i = 0; i < cmd_len; ++i ) { hsai_printf( "%#_010x  ", pword[i] ); }
		hsai_printf( "\n" );

		hsai_trace( "AHCI port %d cmd-slot %d PRDT (%p):", _port_id, cmd_slot, tbl->prdt );
		u32 prd_cnt = cl->headers[cmd_slot].prdtl;

		for ( u32 i = 0; i < prd_cnt; ++i )
		{
			p	  = (volatile u8 *) &tbl->prdt[i];
			pword = (volatile u32 *) p;
			hsai_printf( "prd %d :  %#_010x  %#_010x  %#_010x  %#_010x\n", i, pword[0], pword[1],
						 pword[2], pword[3] );
		}
	}

} // namespace hsai

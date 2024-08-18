//
// Created by Li Shuang ( pseudonym ) on 2024-08-17
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "ahci_port_driver_ls.hh"

#include <ata/ata.hh>
#include <device_manager.hh>
#include <hsai_global.hh>
#include <hsai_log.hh>
#include <mem/virtual_memory.hh>


using namespace loongarch::qemu2k1000;


int AhciPortDriverLs::read_blocks_sync( long start_block, long block_count,
										hsai::BufferDescriptor *buf_list, int buf_count )
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

	// debug_print_cmd_head();
	// debug_print_cmd_table( 0 );
	// while ( !finish );
	while ( _task_busy() || _cmd_slot_busy( _default_cmd_slot ) );

	// hsai_trace( "print AHCI port regs" );
	// volatile u32 *preg = (volatile u32 *) _regs;
	// for ( int i = 0; i < 18; i++ )
	// {
	// 	hsai_printf( "%_08x ", preg[i] );
	// 	if ( i % 4 == 3 ) hsai_printf( "\n" );
	// }
	// while ( _cmd_slot_busy( _default_cmd_slot ) )
	// {
	// 	for ( long i = 100'000'000; i >= 0; --i );

	// 	hsai_trace( "print AHCI port regs" );
	// 	volatile u32 *preg = (volatile u32 *) _regs;
	// 	for ( int i = 0; i < 18; i++ )
	// 	{
	// 		hsai_printf( "%_08x ", preg[i] );
	// 		if ( i % 4 == 3 ) hsai_printf( "\n" );
	// 	}
	// }


	return 0;
}

int AhciPortDriverLs::read_blocks( long start_block, long block_count,
								   hsai::BufferDescriptor *buf_list, int buf_count )
{
	hsai_panic( "not implement" );
	while ( 1 );
}

int AhciPortDriverLs::write_blocks_sync( long start_block, long block_count,
										 hsai::BufferDescriptor *buf_list, int buf_count )
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

int AhciPortDriverLs::write_blocks( long start_block, long block_count,
									hsai::BufferDescriptor *buf_list, int buf_count )
{
	hsai_panic( "not implement" );
	while ( 1 );
}

AhciPortDriverLs::AhciPortDriverLs( const char *lock_name, int port_id, void *base_addr,
									void *cmd_list, void *fis_base, void *cmd_table )
{
	_regs			  = (hsai::AhciPortReg *) base_addr;
	_cmd_lst		  = (hsai::AhciCmdList *) cmd_list;
	_rev_fis		  = (hsai::AhciRevFis *) fis_base;
	_cmd_tbl		  = (hsai::AhciCmdTbl *) cmd_table;
	_port_id		  = port_id;
	_default_cmd_slot = ahci_port_default_cmd_slot;
	_call_back		  = std::bind( &AhciPortDriverLs::ahci_port_default_call_back, this );

	static char _default_dev_name[] = "hd?";
	for ( ulong i = 0; i < sizeof _default_dev_name; ++i ) _dev_name[i] = _default_dev_name[i];

	// 关闭端口以初始化

	if ( _regs->cmd & ( hsai::ahci_port_cmd_st_m | hsai::ahci_port_cmd_cr_m |
						hsai::ahci_port_cmd_fre_m | hsai::ahci_port_cmd_fr_m ) )
	{
		if ( ( _regs->cmd & hsai::ahci_port_cmd_st_m ) != 0 )
		{
			_regs->cmd &= ~hsai::ahci_port_cmd_st_m;
			while ( ( _regs->cmd & hsai::ahci_port_cmd_cr_m ) != 0 );
		}

		if ( _regs->cmd & ( hsai::ahci_port_cmd_fre_m ) )
		{
			_regs->cmd &= ~hsai::ahci_port_cmd_fre_m;
			while ( ( _regs->cmd & hsai::ahci_port_cmd_fr_m ) != 0 );
		}
	}

	if ( _regs->cmd & ( hsai::ahci_port_cmd_cr_m | hsai::ahci_port_cmd_fr_m ) )
	{
		hsai_panic( "\e[5m" "端口 %d 关闭失败!", port_id );
	}

	// 进行端口重置

	_regs->sctl = 0x1;
	for ( int ii = 1'000'000; ii >= 0; --ii );
	_regs->sctl = 0x0;
	while ( ( ( _regs->ssts & hsai::ahci_port_ssts_det_m ) >> hsai::ahci_port_ssts_det_s ) != 0x3 )
		if ( ( ( _regs->ssts & hsai::ahci_port_ssts_det_m ) >> hsai::ahci_port_ssts_det_s ) == 0x1 ) break;
	_regs->serr = _regs->serr;

	u64 tmp;

	// 配置 command list 地址

	tmp			= hsai::k_mem->to_dma( (ulong) _cmd_lst );
	_regs->clb	= (u32) ( tmp & 0xFFFF'FFFFUL );
	_regs->clbu = (u32) ( tmp >> 32 );

	// 配置 received FIS 地址

	tmp		   = hsai::k_mem->to_dma( (ulong) _rev_fis );
	_regs->fb  = (u32) ( tmp & 0xFFFF'FFFFUL );
	_regs->fbu = (u32) ( tmp >> 32 );

	// 打开接收FIS使能

	_regs->cmd |= hsai::ahci_port_cmd_fre_m;
	while ( ( _regs->cmd & hsai::ahci_port_cmd_fr_m ) == 0 );

	// 配置 command table 地址到默认命令头

	tmp = hsai::k_mem->to_dma( (ulong) _cmd_tbl );

	_cmd_lst->headers[_default_cmd_slot].ctba  = (u32) ( tmp & 0xFFFF'FFFFUL );
	_cmd_lst->headers[_default_cmd_slot].ctbau = (u32) ( tmp >> 32 );

	// 清除中断

	// _regs->is = _regs->is;

	// 清除 error 位

	_regs->serr = _regs->serr;

	// 使能中断

	// _regs->ie |= (u32) -1;
	_regs->ie |= hsai::ahci_port_ie_ufe_m | hsai::ahci_port_ie_sdbe_m | hsai::ahci_port_ie_dse_m |
				 hsai::ahci_port_ie_pse_m | hsai::ahci_port_ie_dhre_m;

	// 启动端口

	_regs->cmd |= hsai::ahci_port_cmd_st_m;
	while ( ( _regs->cmd & hsai::ahci_port_cmd_cr_m ) == 0 );


	// 激活端口

	// _regs->cmd |= hsai::ahci_port_cmd_icc_active_m;
	// while ( ( _regs->cmd & hsai::ahci_port_cmd_icc_m ) != 0 );

	// 等待端口空闲

	while ( _task_busy() );

	// 注册到 HSAI

	_dev_name[2] = 'a' + (char) _port_id;
	for ( auto &pname : _partition_name )
	{
		for ( ulong i = 0; i < sizeof _dev_name; ++i ) pname[i] = _dev_name[i];

		pname[3] = 'p';
		pname[4] = '?';
		pname[5] = 0;
	}

	hsai::k_devm.register_block_device( this, _dev_name );

	hsai_trace( "print AHCI port %d regs (%p): ", _port_id, _regs );
	volatile u32 *preg = (volatile u32 *) _regs;
	for ( int i = 0; i < 18; i++ )
	{
		hsai_printf( "%_08x ", preg[i] );
		if ( i % 4 == 3 ) hsai_printf( "\n" );
	}
	hsai_printf( "\n" );
}

void AhciPortDriverLs::isu_cmd_identify( void *buffer, uint len,
										 std::function<int( void )> callback_handler )
{
	if ( len < 512 )
	{
		hsai_warn( "buffer size is not enough.\n" "identify command would not be issue" );
		return;
	}

	uint slot = _default_cmd_slot;

	// 配置 command head

	hsai::AhciCmdHeader &head = _cmd_lst->headers[slot];
	_fill_command_head( head, 1 );

	// 配置 command FIS

	hsai::SataFisRegH2D *fis = (hsai::SataFisRegH2D *) _cmd_tbl->cmd_fis;
	_fill_command_fis( *fis, hsai::ata_cmd_identify_device, 0, 0, 0, 0 );

	// 配置 PRDT （数据区域表）

	hsai::AhciPrd &prd0 = _cmd_tbl->prdt[0];

	u64 pra		   = hsai::k_mem->to_dma( (ulong) buffer );
	prd0.dba	   = pra;
	prd0.interrupt = 0;
	prd0.dbc	   = len - 1;

	// 设置中断回调函数

	if ( callback_handler )
		_call_back = callback_handler;
	else
		_call_back = std::bind( &AhciPortDriverLs::ahci_port_default_call_back, this );

	// 发布命令

	while ( _cmd_slot_busy( slot ) || _task_busy() );
	_issue_command_slot( slot );
}

void AhciPortDriverLs::isu_cmd_read_dma(
	u64 lba, u16 blk_cnt, uint prd_cnt,
	std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
	std::function<int( void )>											callback_handler )
{
	uint slot = _default_cmd_slot;

	// 配置 command head

	hsai::AhciCmdHeader &head = _cmd_lst->headers[slot];
	_fill_command_head( head, prd_cnt );

	// 配置 command FIS

	hsai::SataFisRegH2D *fis = (hsai::SataFisRegH2D *) _cmd_tbl->cmd_fis;
	_fill_command_fis( *fis, hsai::ata_cmd_read_dma, lba, blk_cnt, 0, 1 << 6 );

	// 配置 PRDT （数据区域表）

	hsai::AhciPrd *prd;
	uint64		   dba;
	uint32		   dbc;
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
		prd->interrupt = 0;
		prd->resv1	   = 0;
		prd->resv2	   = 0;
	}

	// 设置中断回调函数

	if ( callback_handler )
		_call_back = callback_handler;
	else
		_call_back = std::bind( &AhciPortDriverLs::ahci_port_default_call_back, this );

	// 发布命令

	while ( _cmd_slot_busy( slot ) || _task_busy() );
	_issue_command_slot( slot );
}

void AhciPortDriverLs::isu_cmd_write_dma(
	uint64 lba, uint64 blk_cnt, uint prd_cnt,
	std::function<void( uint prd_i, uint64 &pr_base, uint32 &pr_size )> set_prd_handler,
	std::function<int( void )>											callback_handler )
{
	uint slot = _default_cmd_slot;

	// 配置 command head

	hsai::AhciCmdHeader &head = _cmd_lst->headers[slot];
	_fill_command_head( head, prd_cnt );

	// 配置 command FIS

	hsai::SataFisRegH2D *fis = (hsai::SataFisRegH2D *) _cmd_tbl->cmd_fis;
	_fill_command_fis( *fis, hsai::ata_cmd_write_dma, lba, blk_cnt, 0, 1 << 6 );

	// 配置 PRDT （数据区域表）

	hsai::AhciPrd *prd;
	uint64		   dba;
	uint32		   dbc;
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
		prd->interrupt = 0;
		prd->resv1	   = 0;
		prd->resv2	   = 0;
	}

	// 设置中断回调函数

	if ( callback_handler )
		_call_back = callback_handler;
	else
		_call_back = std::bind( &AhciPortDriverLs::ahci_port_default_call_back, this );

	// 发布命令

	while ( _cmd_slot_busy( slot ) || _task_busy() );
	_issue_command_slot( slot );
}


int AhciPortDriverLs::handle_intr()
{
	u32 tmp = _regs->is;
	// hsai_trace( "AHCI port %d handle intr IS=%#_010x SERR=%#_010x", _port_id, tmp, _regs->serr );
	int rc;

	// {
	// 	u8 *p = (u8 *) _rev_fis->rfis;
	// 	hsai_trace( "print port %d FIS-D2H data", _port_id );
	// 	hsai_printf( BLUE_COLOR_PRINT
	// 				 "buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
	// 	for ( uint i = 0; i < sizeof _rev_fis->rfis; ++i )
	// 	{
	// 		if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
	// 		hsai_printf( "%B ", p[i] );
	// 		if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
	// 	}
	// 	hsai_printf( "\n" );
	// }

	if ( tmp & hsai::ahci_port_is_dhrs_m )
	{
		rc		   = _call_back();
		_regs->is  = hsai::ahci_port_is_dhrs_m;
		tmp		  &= ~hsai::ahci_port_is_dhrs_m;
	}

	if ( tmp & hsai::ahci_port_is_pss_m )
	{
		rc		   = _call_back();
		_regs->is  = hsai::ahci_port_is_pss_m;
		tmp		  &= ~hsai::ahci_port_is_pss_m;
	}

	if ( tmp )
	{
		hsai_error( "AHCI port %d : unknown intr (is=%p)", _port_id, tmp );

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
		hsai_printf( BLUE_COLOR_PRINT
					 "buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
		for ( uint i = 0; i < sizeof _rev_fis->rfis; ++i )
		{
			if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
			hsai_printf( "%B ", p[i] );
			if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
		}
		hsai_printf( "\n" );

		_regs->is = tmp;

		if ( _regs->serr ) _regs->serr = _regs->serr;

		rc = -100;
	}

	return rc;
}


void AhciPortDriverLs::debug_print_cmd_head()
{
	volatile u32			   *preg;
	volatile hsai::AhciCmdList *cl = (volatile hsai::AhciCmdList *) hsai::k_mem->to_io(
		_regs->clb + ( (ulong) _regs->clbu << 32 ) );
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

void AhciPortDriverLs::debug_print_cmd_table( uint cmd_slot )
{
	volatile hsai::AhciCmdList *cl = (volatile hsai::AhciCmdList *) hsai::k_mem->to_io(
		_regs->clb + ( (ulong) _regs->clbu << 32 ) );
	volatile hsai::AhciCmdTbl *tbl = (volatile hsai::AhciCmdTbl *) hsai::k_mem->to_io(
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

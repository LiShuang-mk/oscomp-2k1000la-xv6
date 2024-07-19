//
// Created by Li Shuang ( pseudonym ) on 2024-07-18 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "ata/ahci_port_driver.hh"
#include "memory_interface.hh"
#include "hsai_global.hh"
#include "device_manager.hh"
#include "mem/virtual_memory.hh"
#include "hsai_log.hh"

namespace hsai
{
	long AhciPortDriver::get_block_size()
	{
		return 0;

	}

	int AhciPortDriver::read_blocks_sync( long start_block, long block_count, void * buf_list, int buf_count )
	{
		return 0;
	}

	int AhciPortDriver::read_blocks( long start_block, long block_count, void * buf_list, int buf_count )
	{
		return 0;
	}

	int AhciPortDriver::write_blocks_sync( long start_block, long block_count, void * buf_list, int buf_count )
	{
		return 0;
	}

	int AhciPortDriver::write_blocks( long start_block, long block_count, void * buf_list, int buf_count )
	{
		return 0;
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

		// 配置 received FIS 地址

		_regs->fb = ( u32 ) ( u64 ) k_mem->to_dma( ( ulong ) _rev_fis );
		_regs->fbu = ( u32 ) ( ( u64 ) k_mem->to_dma( ( ulong ) _rev_fis ) >> 32 );

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

		ahci_port_change_name_number( ( char * ) _dev_name, _port_id );
		k_devm.register_block_device( this, _dev_name );
	}

} // namespace hsai

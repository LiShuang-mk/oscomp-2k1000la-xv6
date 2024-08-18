//
// Created by Li Shuang ( pseudonym ) on 2024-08-17
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "ahci_driver_ls.hh"

#include <device_manager.hh>
#include <hsai_global.hh>
#include <mbr.hh>
#include <mem/virtual_memory.hh>
#include <memory_interface.hh>

#include "qemu_2k1000.hh"

using namespace loongarch::qemu2k1000;

// 按一个页面 4KiB 计算，每个 command list 大小是 1024-bytes
// 8*4K页共容纳 8*4096/1024=32 个 command list
constexpr u64 ahci_command_list_addr = ( 192 * _1M + 0 * hsai::page_size ) | dmwin::win_1;

// 每个 received FIS 大小为 256-bytes
// 2*4K页共容纳 2*4096/256=32 个 received FIS
constexpr u64 ahci_received_fis_addr = ( 192 * _1M + 8 * hsai::page_size ) | dmwin::win_1;

// 每个 command table 暂定为恰好一个页面大小，其中每个 PRD 大小是 32-bytes
// 4K页共容纳 (4096-128)/16=248 个 PRD
constexpr u64 ahci_command_table_addr = ( 192 * _1M + 10 * hsai::page_size ) | dmwin::win_1;

AhciDriverLs::AhciDriverLs( const char *lock_name, void *base_addr )
{
	_lock.init( lock_name );
	_regs		 = (hsai::AhciMemReg *) hsai::k_mem->to_io( (ulong) base_addr );
	_port_num	 = 0;
	_port_bitmap = 0;

	// 关闭中断

	_regs->generic.ghc &= ~hsai::ahci_ghc_ie_m;

	// 检查 AHCI mode

	if ( ( _regs->generic.ghc & hsai::ahci_ghc_ae_m ) == 0 )
		_regs->generic.ghc |= hsai::ahci_ghc_ae_m;

	// 获取端口位图

	u32 port_bit_map = _regs->generic.pi;

	// 清空 command list, received FIS, command Table

	hsai::AhciCmdList *cl  = (hsai::AhciCmdList *) ahci_command_list_addr;
	hsai::AhciRevFis  *rf  = (hsai::AhciRevFis *) ahci_received_fis_addr;
	hsai::AhciCmdTbl  *ct  = (hsai::AhciCmdTbl *) ahci_command_table_addr;
	u8				  *u8p = nullptr;

	u8p = (u8 *) cl;
	for ( u64 *p = (u64 *) u8p, i = 0; i < 32 * sizeof( hsai::AhciCmdList ) / sizeof( u64 ); ++i )
		p[i] = 0;
	u8p = (u8 *) rf;
	for ( u64 *p = (u64 *) u8p, i = 0; i < 32 * sizeof( hsai::AhciRevFis ) / sizeof( u64 ); ++i )
		p[i] = 0;
	u8p = (u8 *) ct;
	for ( u64 *p = (u64 *) u8p, i = 0; i < 32 * sizeof( hsai::AhciCmdTbl ) / sizeof( u64 ); ++i )
		p[i] = 0;

	// 逐个检测端口并初始化

	u32 tmp;


	for ( u32 l = 1, i = 0; l != 0; l <<= 1, i++ )
	{
		// 未被硬件实现的端口
		if ( ( l & port_bit_map ) == 0 ) continue;

		// 没有建立物理链接的端口
		tmp = ( _regs->ports[i].ssts & hsai::ahci_port_ssts_det_m ) >> hsai::ahci_port_ssts_det_s;
		if ( tmp != 0x3 ) continue;

		// 只初始化 ATA 设备（不包含ATAPI）
		tmp = _regs->ports[i].sig;
		if ( tmp != hsai::sata_sig_ata ) continue;

		// 初始化端口，端口设备依次被命名为 hda, hdb, hdc ...

		new ( &_port_drivers[i] )
			AhciPortDriverLs( "AHCI port driver", i, (void *) &_regs->ports[i], (void *) &cl[i],
							  (void *) &rf[i], (void *) &ct[i] );

		_port_num++;
		_port_bitmap |= l;
	}

	_regs->generic.is = _regs->generic.is;

	// 打开中断

	_regs->generic.ghc |= hsai::ahci_ghc_ie_m;

	// 注册到 HSAI
	hsai::k_devm.register_device( this, "AHCI driver" );

	{
		u32 *preg;
		preg = (u32 *) _regs;
		hsai_trace( "print AHCI host regs" );
		for ( int i = 0; i < 11; i++ )
		{
			hsai_printf( "%_08x ", preg[i] );
			if ( i % 4 == 3 ) hsai_printf( "\n" );
		}
		hsai_printf( "\n" );
	}
}

int AhciDriverLs::handle_intr()
{
	int i = 0, res = 0;
	u32 tmp = _regs->generic.is;
	// hsai_info( "SATA intr, IS=%#_010x", tmp );
	for ( u32 l = 1; l != 0; l <<= 1, i++ )
	{
		if ( ( l & _port_bitmap ) == 0 ) continue;
		if ( l & tmp )
		{
			int rc			  = _port_drivers[i].handle_intr();
			_regs->generic.is = l;
			if ( rc < 0 ) return rc;
			res++;
		}
	}
	_regs->generic.is = tmp;
	return res;
}


void AhciDriverLs::identify_device()
{
	bool cmd_finish = false;

	[[maybe_unused]] u32 *id_data	= (u32 *) hsai::alloc_pages( 1 );
	[[maybe_unused]] auto call_back = [&]() -> int
	{
		cmd_finish = true;
		return 0;
	};

	int i		 = 0;
	u8 *mbr_data = new u8[600]; // actually just use 512-bytes

	hsai::BufferDescriptor bdp	= { .buf_addr = (u64) mbr_data, .buf_size = 600 };
	int					   bcnt = 1;

	for ( u32 l = 1; l != 0; l <<= 1, i++ )
	{
		if ( ( l & _port_bitmap ) == 0 ) continue;

		cmd_finish = false;
		_port_drivers[i].isu_cmd_identify( (void *) id_data, hsai::page_size, call_back );
		while ( _port_drivers[i]._task_busy() ||
				_port_drivers[i]._cmd_slot_busy( _port_drivers[i]._default_cmd_slot ) );
		while ( !cmd_finish );
		hsai_trace( "print port %d identify data", i );
		hsai_printf( BLUE_COLOR_PRINT
					 "buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
		for ( uint i = 0; i < 512; ++i )
		{
			if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
			hsai_printf( "%B ", ( (u8 *) id_data )[i] );
			if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
		}
		_analyze_identify_data( i, (u16 *) id_data );

		// _port_drivers[i]._block_size = 512;

		_port_drivers[i].read_blocks_sync( 0, 1, &bdp, bcnt );

		hsai_trace( "print port %d mbr data", i );
		hsai_printf( BLUE_COLOR_PRINT
					 "buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
		for ( uint i = 0; i < 512; ++i )
		{
			if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
			hsai_printf( "%B ", mbr_data[i] );
			if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
		}
		if ( int rc = _check_mbr_partition( mbr_data, i ) < 0 )
		{
			if ( rc == mbr_gpt )
			{
				hsai_panic( "%s设备的硬盘类型是GPT, 但是驱动只支持MBR类型的硬盘! ",
							_port_drivers[i]._dev_name );
				hsai::free_pages( (void *) id_data );
				delete[] mbr_data;
				return;
			}
			else
			{
				hsai_panic( "%s设备硬盘检查失败, 未知的错误码 %d", _port_drivers[i]._dev_name, rc );
				hsai::free_pages( (void *) id_data );
				delete[] mbr_data;
				return;
			}
		}
	}
	hsai::free_pages( (void *) id_data );
	delete[] mbr_data;
}

int AhciDriverLs::_check_mbr_partition( u8 *mbr, uint port_id )
{
	hsai::Mbr				*disk_mbr = (hsai::Mbr *) mbr;
	hsai::DiskPartTableEntry copy_entrys[4] __attribute__( ( aligned( 8 ) ) );

	u8 *pf = (u8 *) disk_mbr->partition_table, *tf = (u8 *) copy_entrys;
	for ( ulong i = 0; i < sizeof copy_entrys; ++i ) { tf[i] = pf[i]; }

	// display all partition
	hsai_info( "打印 MBR 分区表" );
	for ( int i = 0; auto &part : copy_entrys )
	{
		hsai_printf( "分区 %d : 分区状态=%#x 分区类型=%#x 起始LBA=%#x 分区总扇区数=%#x\n", i,
					 part.drive_attribute, part.part_type, part.lba_addr_start, part.sector_count );
		++i;
	}

	for ( int i = 0; i < 4; ++i )
	{
		auto &part = copy_entrys[i];
		if ( part.part_type == 0 ) continue;
		if ( part.part_type == 0xEE ) // this MBR is the protective MBR for GPT disk
		{
			return mbr_gpt;
		}

		if ( part.part_type == hsai::mbr_part_ext )
		{
			AhciPortDriverLs &port	   = _port_drivers[port_id];
			port._partition_name[i][4] = '0' + i;

			new ( &port._disk_partition[i] ) hsai::DiskPartitionDevice(
				(hsai::BlockDevice *) &port, part.lba_addr_start,
				(const char *) port._partition_name[i], hsai::mbr_part_ext );

			hsai::k_devm.register_block_device( (hsai::BlockDevice *) &port._disk_partition[i],
												(const char *) port._partition_name[i] );
		}

		else if ( part.part_type == hsai::mbr_part_fat32_chs_dos ||
				  part.part_type == hsai::mbr_part_fat32_lba_dos ||
				  part.part_type == hsai::mbr_part_fat32_windows )
		{
			hsai_warn( "fat32 partition not implement" );
			return mbr_fat32;
		}
	}
	return 0;
}


int AhciDriverLs::_analyze_identify_data( uint port_id, u16 *id_data )
{
	if ( port_id > hsai::ahci_max_port_num )
	{
		hsai_error( "AHCI : invalid port id" );
		return -100;
	}
	u16 w106 = id_data[106];
	if ( ( w106 & ( 1U << 14 ) ) == 0 || ( w106 & ( 1U << 15 ) ) != 0 )
	{
		hsai_error( "identify AHCI port %d not valid", port_id );
		return -1;
	}
	if ( w106 & ( 1U << 12 ) ) // 逻辑扇区大小超过 256 words
	{
		u16	  w117 = id_data[117], w118 = id_data[118];
		ulong logc_size						= (ulong) ( w117 ) + ( (ulong) w118 << 16 ) /*words*/;
		logc_size						   *= 2; /*bytes*/
		_port_drivers[port_id]._block_size	= logc_size;
	}
	else // 逻辑扇区大小恰为 256 words
	{
		_port_drivers[port_id]._block_size = 512;
	}
	hsai_trace( "AHCI port %d block size is %d <Bytes>", port_id,
				_port_drivers[port_id]._block_size );
	return 0;
}

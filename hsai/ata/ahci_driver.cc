//
// Created by Li Shuang ( pseudonym ) on 2024-07-18
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "ata/ahci_driver.hh"

#include "device_manager.hh"
#include "hsai_global.hh"
#include "hsai_log.hh"
#include "mbr.hh"
#include "mem/virtual_memory.hh"
#include "memory_interface.hh"

namespace hsai
{
	AhciDriver::AhciDriver( const char* lock_name, void* base_addr ) : AhciDriver()
	{
		_lock.init( lock_name );
		_regs = (AhciMemReg*) base_addr;

		// 申请 Received FIS 空间

		AhciRevFis* rev_fis_space = (AhciRevFis*) alloc_pages(
			page_round_up( sizeof( AhciRevFis ) * ahci_max_port_num ) / page_size );
		rev_fis_space = (AhciRevFis*) k_mem->to_io( (ulong) rev_fis_space );

		{
			u8*	 prf = (u8*) rev_fis_space;
			u64* p	 = (u64*) prf;
			long cnt = page_round_up( sizeof( AhciRevFis ) * ahci_max_port_num ) / sizeof( u64 );
			for ( long i = 0; i < cnt; ++i ) p[i] = 0;
		}

		// 申请 Command List 空间

		AhciCmdList* cmd_ls_space = (AhciCmdList*) alloc_pages(
			page_round_up( sizeof( AhciCmdList ) * ahci_max_port_num ) / page_size );
		cmd_ls_space = (AhciCmdList*) k_mem->to_io( (ulong) cmd_ls_space );

		{
			u8*	 pcl = (u8*) cmd_ls_space;
			u64* p	 = (u64*) pcl;
			long cnt = page_round_up( sizeof( AhciCmdList ) * ahci_max_port_num ) / sizeof( u64 );
			for ( long i = 0; i < cnt; ++i ) p[i] = 0;
		}

		// {
		// 	// print command list
		// 	u32* preg;
		// 	u8*	 p = (u8*) cmd_ls_space;
		// 	preg   = (u32*) p;
		// 	hsai_trace( "print AHCI host regs" );
		// 	for ( int i = 0; i < 256; i++ )
		// 	{
		// 		hsai_printf( "%_08x ", preg[i] );
		// 		if ( i % 8 == 7 ) hsai_printf( "\n" );
		// 	}
		// 	hsai_printf( "\n" );
		// }

		// 关闭中断

		_regs->generic.ghc &= ~ahci_ghc_ie_m;

		// >>>>

		{
			u32* preg;
			preg = (u32*) _regs;
			hsai_trace( "print AHCI host regs" );
			for ( int i = 0; i < 11; i++ )
			{
				hsai_printf( "%_08x ", preg[i] );
				if ( i % 4 == 3 ) hsai_printf( "\n" );
			}
			hsai_printf( "\n" );
		}

		// >> 重启设备

		// _regs->generic.ghc |= ahci_ghc_hr_m;
		// while ( _regs->generic.ghc & ahci_ghc_hr_m );

		// _regs->generic.ghc |= ahci_ghc_ae_m;
		// if ( ( _regs->ports[0].cmd & ( ahci_port_cmd_st_m | ahci_port_cmd_cr_m |
		// 							   ahci_port_cmd_fre_m | ahci_port_cmd_fr_m ) ) != 0 ||
		// 	 ( _regs->ports[0].sctl & ahci_port_sctl_det_m ) != 0 )
		// {
		// 	if ( _regs->ports[0].cmd & ahci_port_cmd_st_m )
		// 	{
		// 		_regs->ports[0].cmd &= ~ahci_port_cmd_st_m;
		// 		while ( ( _regs->ports[0].cmd & ahci_port_cmd_cr_m ) != 0 );
		// 	}

		// 	if ( _regs->ports[0].cmd & ( ahci_port_cmd_fre_m ) )
		// 	{
		// 		_regs->ports[0].cmd &= ~ahci_port_cmd_fre_m;
		// 		while ( ( _regs->ports[0].cmd & ahci_port_cmd_fr_m ) != 0 );
		// 	}

		// 	if ( _regs->ports[0].sctl & ahci_port_sctl_det_m )
		// 		hsai_panic( "port 0 sctl.det != 0h" );
		// }

		// _regs->ports[0].cmd |= ahci_port_cmd_sud_m;
		// while ( 1 )
		// {
		// 	u32 tmp = ( _regs->ports[0].ssts & ahci_port_ssts_det_m ) >> ahci_port_ssts_det_s;
		// 	if ( tmp == 0x1 || tmp == 0x3 ) break;
		// }

		// _regs->ports[0].serr = _regs->ports[0].serr;

		// // while ( 1 )
		// // {
		// // 	u32 tmp = ( _regs->ports[0].tfd & ( ahci_port_tfd_sts_bsy_m |
		// ahci_port_tfd_sts_drq_m |
		// // 										ahci_port_tfd_sts_drq_m ) );
		// // 	// hsai_printf( "tfd=%08x ", _regs->ports[0].tfd );
		// // 	if ( tmp == 0 ) break;
		// // }
		// hsai_printf( "\n" );
		// hsai_info( "\e[5m" ">>>> AHCI port 0 ready!" );

		// << 重启设备

		{
			volatile u32* preg;
			preg = (volatile u32*) &_regs->ports[0];
			hsai_trace( "print AHCI port 0 regs (%p)", &( _regs->ports[0] ) );
			for ( int i = 0; i < 18; i++ )
			{
				hsai_printf( "%_08x ", preg[i] );
				if ( i % 4 == 3 ) hsai_printf( "\n" );
			}
			hsai_printf( "\n" );
			hsai_printf( "tfd(%p)=%08x ", &( _regs->ports[0].tfd ), _regs->ports[0].tfd );

			new ( &_port_drivers[0] ) AhciPortDriver( "AHCI port", 0, &_regs->ports[0],
													  &cmd_ls_space[0], &rev_fis_space[0] );

			hsai_trace( "print AHCI port regs" );
			for ( int i = 0; i < 18; i++ )
			{
				hsai_printf( "%_08x ", preg[i] );
				if ( i % 4 == 3 ) hsai_printf( "\n" );
			}
			hsai_printf( "\n" );

			++_port_num;
			_port_bitmap |= 1;

			_regs->generic.is = _regs->generic.is;

			_regs->generic.ghc |= ahci_ghc_ie_m;


			// 注册到 HSAI
			k_devm.register_device( this, "AHCI driver" );

			// [[maybe_unused]] word* id_data = (word*) alloc_pages( 1 );
			// bool				   cmd_finish;
			// [[maybe_unused]] auto  call_back = [&]() -> int
			// {
			// 	cmd_finish = true;
			// 	return 0;
			// };

			// cmd_finish = false;
			// _port_drivers[0].isu_cmd_identify( (void*) id_data, page_size, call_back );
			// // while ( _port_drivers[i]._task_busy() ||
			// // 		_port_drivers[i]._cmd_slot_busy( _port_drivers[i]._default_cmd_slot ) );
			// while ( !cmd_finish );
			// hsai_trace( "print port %d identify data", 0 );
			// hsai_printf(
			// 	BLUE_COLOR_PRINT
			// 	"buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
			// for ( uint i = 0; i < 512; ++i )
			// {
			// 	if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
			// 	hsai_printf( "%B ", ( (u8*) id_data )[i] );
			// 	if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
			// }
		}

		{
			u32* preg;
			preg = (u32*) _regs;
			hsai_trace( "print AHCI host regs" );
			for ( int i = 0; i < 11; i++ )
			{
				hsai_printf( "%_08x ", preg[i] );
				if ( i % 4 == 3 ) hsai_printf( "\n" );
			}
			hsai_printf( "\n" );
		}

		return;
		while ( 1 );

		// <<<<

		// 重启设备

		// _regs->generic.ghc |= ahci_ghc_hr_m;
		// while ( _regs->generic.ghc & ahci_ghc_hr_m );

		hsai_trace( "AHCI CAP : %#_010x", _regs->generic.cap );

		hsai_trace( "AHCI version : %x", _regs->generic.vs );

		// 使能 AHCI

		_regs->generic.ghc |= ahci_ghc_ae_m;

		// 获取端口数量

		ulong pn = ( ( _regs->generic.cap & ahci_cap_np_m ) >> ahci_cap_np_s ) + 1;
		hsai_trace( "AHCI PI num : %d", pn );

		// 获取端口位图

		ulong bpm = _regs->generic.pi;
		hsai_info( "AHCI PI map : %#_010x", bpm );

		// 清中断

		_regs->generic.is = (u32) -1;

		// 逐个检测端口并初始化

		int i = 0;
		u32 tmp;
		for ( u32 l = 1; l != 0; l <<= 1, i++ )
		{
			// 未被硬件实现的端口
			if ( ( l & bpm ) == 0 && i != 0 ) continue;

			tmp = _regs->ports[i].sig;
			hsai_trace( "AHCI 端口 %d 签名 %#_010x", i, tmp );
			hsai_trace( "AHCI 端口 %d SSTS (%p) = %#_010x", i, &_regs->ports[i].ssts,
						_regs->ports[i].ssts );

			// 未建立物理链接的端口
			if ( ( ( _regs->ports[i].ssts & ahci_port_ssts_det_m ) >> ahci_port_ssts_det_s ) !=
				 0x3 )
			{
				if ( ( ( _regs->ports[i].ssts & ahci_port_ssts_det_m ) >> ahci_port_ssts_det_s ) !=
					 0x1 )
					continue;

				hsai_trace( "AHCI 端口 %d 未建立物理链接", i );
				hsai_trace( "AHCI 端口 %d SCTL = %#_010x", i, _regs->ports[i].sctl );

				hsai_trace( "AHCI 端口 %d CMD = %#_010x", i, _regs->ports[i].cmd );
				// _regs->ports[i].cmd &= ~ahci_port_cmd_st_m;
				// while ( _regs->ports[i].cmd & ahci_port_cmd_cr_m );

				_regs->ports[i].sctl |= 1;
				for ( int ii = 1'000'000; ii > 0; --ii );
				_regs->ports[i].sctl &= ~0xF;

				// _regs->ports[i].cmd |= ahci_port_cmd_st_m;
				// while ( !( _regs->ports[i].cmd & ahci_port_cmd_cr_m ) );

				hsai_trace( "AHCI 端口 %d reset后 SSTS = %#_010x", i, _regs->ports[i].ssts );

				if ( ( ( _regs->ports[i].ssts & ahci_port_ssts_det_m ) >> ahci_port_ssts_det_s ) !=
					 0x3 )
				{
					hsai_panic( "AHCI 端口 %d 无法建立物理链接", i );
				}

				continue;
			}

			// 只初始化 ATA 设备（不包含ATAPI）
			tmp = _regs->ports[i].sig;
			if ( tmp != sata_sig_ata ) continue;
			hsai_trace( "port\t%d sig : %x", i, tmp );

			// 初始化端口，端口设备依次被命名为 hda, hdb, hdc ...

			new ( &_port_drivers[i] ) AhciPortDriver( "AHCI port", i, &_regs->ports[i],
													  &cmd_ls_space[i], &rev_fis_space[i] );

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
		hsai_info( "SATA intr, IS=%#_010x", tmp );
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
		// _regs->generic.is = tmp;
		return res;
	}

	void AhciDriver::identify_device()
	{
		[[maybe_unused]] word* id_data = (word*) alloc_pages( 1 );
		bool				   cmd_finish;
		[[maybe_unused]] auto  call_back = [&]() -> int
		{
			cmd_finish = true;
			return 0;
		};
		int i		 = 0;
		u8* mbr_data = new u8[600]; // actually just use 512-bytes

		BufferDescriptor bdp  = { .buf_addr = (u64) mbr_data, .buf_size = 600 };
		int				 bcnt = 1;
		for ( u32 l = 1; l != 0; l <<= 1, i++ )
		{
			if ( ( l & _port_bitmap ) == 0 ) continue;

			// cmd_finish = false;
			// _port_drivers[i].isu_cmd_identify( (void*) id_data, page_size, call_back );
			// while ( _port_drivers[i]._task_busy() ||
			// 		_port_drivers[i]._cmd_slot_busy( _port_drivers[i]._default_cmd_slot ) );
			// // while ( !cmd_finish );
			// hsai_trace( "print port %d identify data", i );
			// hsai_printf(
			// 	BLUE_COLOR_PRINT
			// 	"buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" CLEAR_COLOR_PRINT );
			// for ( uint i = 0; i < 512; ++i )
			// {
			// 	if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
			// 	hsai_printf( "%B ", ( (u8*) id_data )[i] );
			// 	if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
			// }
			// _analyze_identify_data( i, id_data );

			_port_drivers[i]._block_size = 512;

			_port_drivers[i].read_blocks_sync( 0, 1, &bdp, bcnt );

			hsai_trace( "print port %d mbr data", i );
			hsai_printf(
				BLUE_COLOR_PRINT
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
					free_pages( (void*) id_data );
					delete[] mbr_data;
					return;
				}
				else
				{
					hsai_panic( "%s设备硬盘检查失败, 未知的错误码 %d", _port_drivers[i]._dev_name,
								rc );
					free_pages( (void*) id_data );
					delete[] mbr_data;
					return;
				}
			}
		}
		free_pages( (void*) id_data );
		delete[] mbr_data;
	}

	///////////////////////////////// private helper function /////////////////////////////////

	int AhciDriver::_analyze_identify_data( uint port_id, word* id_data )
	{
		if ( port_id > ahci_max_port_num )
		{
			hsai_error( "AHCI : invalid port id" );
			return -100;
		}
		word w106 = id_data[106];
		if ( ( w106 & ( 1U << 14 ) ) == 0 || ( w106 & ( 1U << 15 ) ) != 0 )
		{
			hsai_error( "identify AHCI port %d not valid", port_id );
			return -1;
		}
		if ( w106 & ( 1U << 12 ) ) // 逻辑扇区大小超过 256 words
		{
			word  w117 = id_data[117], w118 = id_data[118];
			ulong logc_size	 = (ulong) ( w117 ) + ( (ulong) w118 << 16 ) /*words*/;
			logc_size		*= 2; /*bytes*/
			_port_drivers[port_id]._block_size = logc_size;
		}
		else // 逻辑扇区大小恰为 256 words
		{
			_port_drivers[port_id]._block_size = 512;
		}
		hsai_trace( "AHCI port %d block size is %d <Bytes>", port_id,
					_port_drivers[port_id]._block_size );
		return 0;
	}

	int AhciDriver::_check_mbr_partition( u8* mbr, uint port_id )
	{
		Mbr*			   disk_mbr = (Mbr*) mbr;
		DiskPartTableEntry copy_entrys[4] __attribute__( ( aligned( 8 ) ) );

		u8 *pf = (u8*) disk_mbr->partition_table, *tf = (u8*) copy_entrys;
		for ( ulong i = 0; i < sizeof copy_entrys; ++i ) { tf[i] = pf[i]; }

		// display all partition
		hsai_info( "打印 MBR 分区表" );
		for ( int i = 0; auto& part : copy_entrys )
		{
			hsai_printf( "分区 %d : 分区状态=%#x 分区类型=%#x 起始LBA=%#x 分区总扇区数=%#x\n", i,
						 part.drive_attribute, part.part_type, part.lba_addr_start,
						 part.sector_count );
			++i;
		}

		for ( int i = 0; i < 4; ++i )
		{
			auto& part = copy_entrys[i];
			if ( part.part_type == 0 ) continue;
			if ( part.part_type == 0xEE ) // this MBR is the protective MBR for GPT disk
			{
				return mbr_gpt;
			}

			if ( part.part_type == mbr_part_ext )
			{
				AhciPortDriver& port	   = _port_drivers[port_id];
				port._partition_name[i][4] = '0' + i;

				new ( &port._disk_partition[i] )
					DiskPartitionDevice( (BlockDevice*) &port, part.lba_addr_start,
										 (const char*) port._partition_name[i], mbr_part_ext );

				k_devm.register_block_device( (BlockDevice*) &port._disk_partition[i],
											  (const char*) port._partition_name[i] );
			}

			if ( part.part_type == mbr_part_fat32_chs_dos ||
				 part.part_type == mbr_part_fat32_lba_dos ||
				 part.part_type == mbr_part_fat32_windows )
			{
				hsai_warn( "fat32 partition not implement" );
				return mbr_fat32;
			}
		}
		return 0;
	}

} // namespace hsai

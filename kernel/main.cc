#include "console.hh"
#include "hal/loongarch.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/cpu.hh"
#include "hal/disk/mbr.hh"
#include "fs/dev/pci_driver.hh"
#include "fs/dev/ahci_controller.hh"
#include "fs/dev/sata_driver.hh"
#include "fs/fat/fat32.hh"
#include "fs/ext4/super_block.hh"
#include "fs/ext4/block_group_descriptor.hh"
#include "fs/ext4/index_node.hh"
#include "fs/buffer_manager.hh"
#include "tm/timer_manager.hh"
#include "im/exception_manager.hh"
#include "im/interrupt_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include "mm/heap_memory_manager.hh"
#include "pm/process_manager.hh"
#include "pm/shmmanager.hh"
#include "klib/printer.hh"
#include "klib/common.hh"

// entry.S needs one stack per CPU.
__attribute__( ( aligned( 16 ) ) ) char stack0[ 4096 * NUMCPU ];

void test_sata();
void test_buffer();

int main()
{
	if ( loongarch::Cpu::read_tp() == 0 )
	{
		// console init 
		k_console.init( "console" );

		// printf init 
		kernellib::k_printer.init( &k_console, "printer" );
		log_info( "Hello World!\n" );


		uint64 tmp = loongarch::Cpu::read_csr( loongarch::csr::crmd );
		log_trace( "crmd: %p", tmp );

		// physical memory init 
		mm::k_pmm.init( "physical memory manager",
			loongarch::qemuls2k::memory::mem_start,
			loongarch::qemuls2k::memory::mem_end );
		log_info( "pmm init" );

		// process init 
		pm::k_pm.init( "next pid", "wait lock" );
		log_info( "pm init" );

		// vm init 
		mm::k_vmm.init( "virtual memory manager " );
		log_info( "vm init" );

		// timer init 
		tm::k_tm.init( "timer manager" );
		log_info( "tm init" );

		// interrupt init 
		im::k_im.init( "interrupt init" );
		log_info( "im init" );

		// exception init 
		im::k_em.init( "exception manager " );
		log_info( "em init" );
		// while ( 1 );

		// sharemem init
		pm::k_shmManager.init( "shm lock" );
		log_info( "shm init" );

		// log_trace(
		// 	"hmm初始化前跟踪空闲物理页 : %d",
		// 	mm::k_pmm.trace_free_pages_count()
		// );
		mm::k_hmm.init( "k_heap manager" );
		log_info( "hmm init" );
		// log_trace(
		// 	"hmm初始化后跟踪空闲物理页 : %d",
		// 	mm::k_pmm.trace_free_pages_count()
		// );

		// uint32 apbh[ 64 ];
		// uint64 addr = ( ( 0xFE0UL << 28 ) | ( 0x0UL << 16 ) | ( 0x2UL << 11 ) | ( 0x0UL << 8 ) );
		// printf( "addr: \n%p\n", addr | loongarch::qemuls2k::dmwin::win_1 );
		// printf( "head: \n" );
		// volatile uint32 * p = ( volatile uint32 * ) ( addr | loongarch::qemuls2k::dmwin::win_1 );
		// for ( int i = 0; i < 16; i++, p++ )
		// 	apbh[ i ] = *p;
		// for ( int i = 0; i < 16; i++ )
		// 	printf( "%x\n", apbh[ i ] );

		// volatile uint32 *p = ( volatile uint32 * ) ( 0x400e0000 | loongarch::qemuls2k::dmwin::win_1 );
		// for ( int i = 0; i < 0x200; i += 4, p++ )
		// 	printf( "%x\t\t%p\n", i, *p );

		dev::ahci::k_ahci_ctl.init( "ahci controller" );

		dev::pci::k_pci_driver.init( "pci driver" );

		// 这个测试里面包含修改硬盘数据的敏感操作
		// 使用前先备份2kfs.img或sdcard.img
		// test_sata();

		log_trace(
			"bufm 初始化前跟踪空闲物理页 : %d",
			mm::k_pmm.trace_free_pages_count()
		);
		fs::k_bufm.init( "buffer manager" );
		log_info( "bufm init" );
		log_trace(
			"bufm 初始化后跟踪空闲物理页 : %d",
			mm::k_pmm.trace_free_pages_count()
		);

		// test_noreturn();

		test_buffer();

		// fs::Buffer buf = fs::k_bufm.get_buffer( 0, 0 );
		// log_trace( "测试 buffer : %p", buf.debug_get_buffer_base() );
		// // fs::k_bufm.release_buffer( buf );
		// buf = fs::k_bufm.get_buffer( 0, 1024 );
		// log_trace( "测试 buffer : %p", buf.debug_get_buffer_base() );
		// // fs::k_bufm.release_buffer( buf );

		int tm, tn;
		tm = 0;
		tn = 1;
		assert( tm == tn, "测试assert, 需求 %d, 而输入 %d", tn, tm );

		while ( 1 );

		printf( "\n" );
		log_trace( "simple trace" );
		log_trace( "test\n%s", "trace" );
		log_info( "test info" );
		log_warn( "test warn " );
		log_error( "test error" );
		// log_panic( "test panic" );
		assert( 0, "" );

		log_info( "Kernel not complete. About to enter loop. " );
		while ( 1 ); // stop here
	}
	else
		while ( 1 );

	return 0;
}

void* buffer1;
void* buffer2;
void test_sata_handle_identify()
{
	log_info( "<----identify命令执行成功---->" );
	log_info(
		"打印收到的数据\n"
		"\b\b\b\b________________________________\n"
	);

	printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	uchar *p = ( uchar * ) buffer1;
	for ( uint i = 0; i < 512; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}

	log_trace(
		"- word 106 : %x\n"
		"- logical sector size : %d\n",
		*( ( uint16* ) buffer1 + 106 ),
		*( uint32* ) ( ( uint16* ) buffer1 + 117 )
	);
}

disk::Mbr mbr;
bool mbr_init = false;
void test_sata_call_back()
{
	log_info( "<----中断回调---->" );
	log_info(
		"打印收到的数据\n"
		"\b\b\b\b________________________________\n"
	);

	printf( "  \t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	uchar *p = ( uchar * ) &mbr;
	for ( uint i = 0; i < 512; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}

	for ( int i = 0; i < 4; ++i )
	{
		disk::DiskPartTableEntry *dpte = &mbr.partition_table[ i ];
		if ( dpte->part_type != 0 )
		{
			log_trace(
				"||=> 硬盘分区%d\n"
				"  分区属性:           %B\n"
				"  分区起始地址(CHS):  %x\n"
				"  分区类型:           %B\n"
				"  分区结束地址(CHS):  %x\n"
				"  分区开始扇区(LBA):  %x\n"
				"  分区扇区数量:       %d",
				i,
				dpte->drive_attribute,
				dpte->chs_addr_start,
				dpte->part_type,
				dpte->chs_addr_last,
				dpte->lba_addr_start,
				dpte->sector_count
			);
		}
	}
	mbr_init = true;
	return;

	struct fs::fat::Fat32Dbr* dbr = ( struct fs::fat::Fat32Dbr* ) buffer1;
	if ( compare( dbr->ebpb.system_id, "FAT32 ", 6 ) == 0 )
	{
		log_trace(
			"扇区大小:              %d bytes\n"
			"簇大小:                %d sectors\n"
			"保留扇区数:            %d\n"
			"FAT 数量:              %d\n"
			"根目录条目数量:        %d\n"
			"硬盘介质类型:          %x\n"
			"一个磁道的扇区数:      %d\n"
			"磁头的数量:            %d\n"
			"隐藏扇区数量:          %d\n"
			"逻辑分区中的扇区数量:  %d\n",
			dbr->bpb.bytes_per_sector,
			dbr->bpb.sectors_per_cluster,
			dbr->bpb.reserved_sector_count,
			dbr->bpb.table_count,
			dbr->bpb.root_entry_count,
			dbr->bpb.media_type,
			dbr->bpb.sectors_per_track,
			dbr->bpb.head_side_count,
			dbr->bpb.hidden_sector_count,
			dbr->bpb.total_sectors_32
		);
	}
}

fs::ext4::SuperBlock ext4_super_block;
void test_sata_call_back_2()
{
	log_info( "<----中断回调2---->" );
	log_info(
		"打印收到的数据\n"
		"\b\b\b\b________________________________\n"
	);

	printf( "  \t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	uchar *p = ( uchar * ) &ext4_super_block;
	for ( uint i = 0; i < 1024; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}
	printf(
		"||====> EXT4 超级块:\n"
		"  inode总数:            %d\n"
		"  block总数:            %d\n"
		"  block大小:            %d bytes\n"
		"  每个块组的block数:    %d\n"
		"  每个块组的inode数:    %d\n"
		"  魔术签名:             %x\n"
		"  版本号:               %d.%d\n",
		ext4_super_block.inodes_count,
		( uint64 ) ext4_super_block.blocks_count_lo + ( ( uint64 ) ext4_super_block.blocks_count_hi << 32 ),
		math::power( 2, ext4_super_block.log_block_size + 10 ),
		ext4_super_block.blocks_per_group,
		ext4_super_block.inodes_per_group,
		ext4_super_block.magic,
		ext4_super_block.rev_level,
		ext4_super_block.minor_rev_level
	);
}

void test_sata()
{
	buffer1 = mm::k_pmm.alloc_page();
	// mm::k_pmm.clear_page( buffer1 );
	buffer2 = mm::k_pmm.alloc_page();

	bool busy;

	dev::ahci::k_ahci_ctl.isu_cmd_read_dma( 0, 0, 512, 2,
		[ & ] ( uint i, uint64& prb, uint32& prs )->void
	{
		busy = true;
		if ( i == 0 )
			prb = ( uint64 ) buffer1;
		else if ( i == 1 )
			prb = ( uint64 ) buffer2;
		else return;
		prs = 256;
	},
		[ & ] () -> void
	{
		log_info( "[test] 先将0号扇区读取出来" );
		uchar *p;
		printf( "buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
		p = ( uchar * ) buffer1;
		for ( uint i = 0; i < 256; ++i )
		{
			if ( i % 0x10 == 0 )
				printf( "%B%B\t", i >> 8, i );
			printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				printf( "\n" );
		}
		p = ( uchar * ) buffer2;
		printf( "buf1\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
		for ( uint i = 0; i < 256; ++i )
		{
			if ( i % 0x10 == 0 )
				printf( "%B%B\t", i >> 8, i );
			printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				printf( "\n" );
		}
		busy = false;
	} );

	while ( busy );

	// 修改一下开头的字符
	// const char str[10] = "123456789";
	const char str[ 10 ] = "\0\0\0\0\0\0\0\0\0";
	for ( uint i = 0; i < sizeof( str ); ++i )
	{
		*( ( char * ) buffer1 + i ) = str[ i ];
	}

	dev::ahci::k_ahci_ctl.isu_cmd_write_dma( 0, 0, 512, 2,
		[ & ] ( uint i, uint64& prb, uint32& prs ) -> void
	{
		busy = true;
		if ( i == 0 )
			prb = ( uint64 ) buffer1;
		else if ( i == 1 )
			prb = ( uint64 ) buffer2;
		else return;
		prs = 256;
	},
		[ & ] () ->void
	{
		log_info( "[test] 硬盘写入完成" );
		busy = false;
	} );

	while ( busy );

	dev::ahci::k_ahci_ctl.isu_cmd_read_dma( 0, 0, 512, 2,
		[ & ] ( uint i, uint64& prb, uint32& prs )->void
	{
		busy = true;
		if ( i == 0 )
			prb = ( uint64 ) buffer1;
		else if ( i == 1 )
			prb = ( uint64 ) buffer2;
		else return;
		prs = 256;
	},
		[ & ] () -> void
	{
		log_info( "[test] 打印修改后的0号扇区" );
		uchar *p;
		printf( "buf0\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
		p = ( uchar * ) buffer1;
		for ( uint i = 0; i < 256; ++i )
		{
			if ( i % 0x10 == 0 )
				printf( "%B%B\t", i >> 8, i );
			printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				printf( "\n" );
		}
		p = ( uchar * ) buffer2;
		printf( "buf1\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
		for ( uint i = 0; i < 256; ++i )
		{
			if ( i % 0x10 == 0 )
				printf( "%B%B\t", i >> 8, i );
			printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				printf( "\n" );
		}
		busy = false;
	} );

	while ( 1 );
}

void test_buffer()
{
	fs::Buffer buf = fs::k_bufm.read_sync( 0, 0 );
	char * p = ( char * ) buf.get_data_ptr();
	log_info( "打印读取到buffer的内容" );
	printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	for ( uint i = 0; i < 512; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}

	disk::Mbr * p_mbr = ( disk::Mbr * ) p;
	uint32 part_lba[ 4 ] = { 0,0,0,0 };
	for ( int i = 0; i < 4; ++i )
	{
		disk::DiskPartTableEntry *dpte = &p_mbr->partition_table[ i ];
		part_lba[ i ] = dpte->lba_addr_start;
		if ( dpte->part_type != 0 )
		{
			log_trace(
				"||=> 硬盘分区%d\n"
				"  分区属性:           %B\n"
				"  分区起始地址(CHS):  %xH\n"
				"  分区类型:           %B\n"
				"  分区结束地址(CHS):  %xH\n"
				"  分区开始扇区(LBA):  %xH\n"
				"  分区扇区数量:       %d",
				i,
				dpte->drive_attribute,
				dpte->chs_addr_start,
				dpte->part_type,
				dpte->chs_addr_last,
				dpte->lba_addr_start,
				dpte->sector_count
			);
		}
	}

	fs::k_bufm.release_buffer_sync( buf );

	if ( part_lba[ 0 ] == 0 )
		while ( 1 );

	buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + 2 );
	p = ( char * ) buf.get_data_ptr();
	log_info( "打印读取到buffer的内容" );
	printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	for ( uint i = 0; i < 512; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}

	[[maybe_unused]] uint32 bpg;	// blocks per group
	[[maybe_unused]] uint32 ipg;	// inodes pre group
	[[maybe_unused]] uint32 jni;	// journal inode number
	[[maybe_unused]] uint64 bs;		// block size
	fs::ext4::SuperBlock * s_b = ( fs::ext4::SuperBlock * ) p;
	bpg = s_b->blocks_per_group;
	ipg = s_b->inodes_per_group;
	jni = s_b->journal_inum;
	bs = math::power( 2, s_b->log_block_size + 10 );
	log_trace(
		"||====> EXT4 超级块:\n"
		"  inode总数:            %d\n"
		"  block总数:            %d\n"
		"  block大小:            %d bytes\n"
		"  每个块组的block数:    %d\n"
		"  每个块组的inode数:    %d\n"
		"  每个inode大小:        %d bytes\n"
		"  为GDT保留的block数:   %d\n"
		"  魔术签名:             %x\n"
		"  版本号:               %d.%d\n"
		"  日志inode:            %d\n",
		s_b->inodes_count,
		( uint64 ) s_b->blocks_count_lo + ( ( uint64 ) s_b->blocks_count_hi << 32 ),
		bs,
		s_b->blocks_per_group,
		s_b->inodes_per_group,
		s_b->inode_size,
		s_b->reserved_gdt_blocks,
		s_b->magic,
		s_b->rev_level,
		s_b->minor_rev_level,
		s_b->journal_inum
	);

	fs::k_bufm.release_buffer_sync( buf );

	buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + bs / dev::sata::k_sata_driver.get_sector_size() );
	// buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + 2 + 2 );
	p = ( char * ) buf.get_data_ptr();
	log_info( "打印读取到buffer的内容" );
	printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	for ( uint i = 0; i < 512; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}

	uint bg_id = 0;
	fs::ext4::BlockGroupDesc * bgd = ( fs::ext4::BlockGroupDesc * ) p;
	bgd += bg_id;
	log_trace(
		"┌──────────────────────────────────┐\n"
		"│           块组%d描述符            │\n"
		"├─────────────────┬────────────────┤\n"
		"│   块位图起始块  │\t%d\t│\n"
		"├─────────────────┼────────────────┤\n"
		"│ iNode位图起始块 │\t%d\t│\n"
		"├─────────────────┼────────────────┤\n"
		"│  iNode表起始块  │\t%d\t│\n"
		"└─────────────────┴────────────────┘\n",
		bg_id,
		( uint64 ) bgd->block_bitmap_lo + ( ( uint64 ) bgd->block_bitmap_hi << 32 ),
		( uint64 ) bgd->inode_bitmap_lo + ( ( uint64 ) bgd->inode_bitmap_hi << 32 ),
		( uint64 ) bgd->inode_table_lo + ( ( uint64 ) bgd->inode_table_hi << 32 )
	);
}
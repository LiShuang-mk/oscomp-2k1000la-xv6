#include "hal/loongarch.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/cpu.hh"
#include "hal/disk/mbr.hh"

#include "fs/device.hh"
#include "fs/file.hh"
#include "fs/dev/console.hh"
#include "fs/dev/pci_driver.hh"
#include "fs/dev/ahci_controller.hh"
#include "fs/dev/sata_driver.hh"
#include "fs/fat/fat32.hh"
#include "fs/fat/fat32_file_system.hh"
#include "fs/ext4/super_block.hh"
#include "fs/ext4/block_group_descriptor.hh"
#include "fs/ext4/index_node.hh"
#include "fs/jbd2/journal_super_block.hh"
#include "fs/buffer_manager.hh"

#include "tm/timer_manager.hh"

#include "im/exception_manager.hh"
#include "im/interrupt_manager.hh"

#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include "mm/heap_memory_manager.hh"

#include "pm/process_manager.hh"
#include "pm/shmmanager.hh"
#include "pm/scheduler.hh"

#include "klib/printer.hh"
#include "klib/common.hh"

#include <bit>

#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

// entry.S needs one stack per CPU.
__attribute__( ( aligned( 16 ) ) ) char stack0[ loongarch::entry_stack_size * NUMCPU ];

void test_sata();
void test_buffer();
void test_fat32();

char tmp_buf[ 4096 ];

extern uint64 _start_u_init;
extern uint64 stext;
extern uint64 etext;

extern "C" {
	extern uint64 _u_init_txts;
	extern uint64 _u_init_txte;
	extern int init_main( void );
}

int main()
{
	if ( loongarch::Cpu::read_tp() == 0 )
	{
		// console init 
		dev::k_console.init( "console" );

		// printf init 
		kernellib::k_printer.init( &dev::k_console, "printer" );
		log_info( "Hello World!\n" );

		log_info( "main addr: %p", &main );


		uint64 tmp = loongarch::Cpu::read_csr( loongarch::csr::crmd );
		log_trace( "crmd: %p", tmp );
		loongarch::Cpu::euen_float();

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

		// timer init 
		tmm::k_tm.init( "timer manager" );
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

		// test_buffer();
		// while ( 1 );


		new ( &fs::fat::k_testcase_fs ) fs::fat::Fat32FileSystem;
		fs::fat::k_testcase_fs.init( 1, 0 );
		log_info( "testcase fs init" );

		dev::k_dm.init();
		log_info( "k_dm init" );

		// test_fat32();
		// while ( 1 );
		// eastl::vector<eastl::string> args;
		// pm::k_pm.exec("test_echo",args);
		// log_info( "text start %p\n", &stext );
		// log_info( "text end   %p\n", &etext ); 

		log_info( "user code start %p", &_start_u_init );
		log_info( "user init_main address %p", ( uint64 ) &init_main );
		log_info( "user code size %d Bytes", ( uint64 ) &_u_init_txte - ( uint64 ) &_u_init_txts );

		pm::k_pm.user_init();
		log_info( "user init" );

		pm::Pcb * np = pm::k_pm.alloc_proc();
		np->_ofile[1] = pm::k_proc_pool[0]._ofile[1];
		loongarch::Cpu::get_cpu()->set_cur_proc(np);

		eastl::vector<eastl::string> args;
		pm::k_pm.exec( "test_echo", args );
		pm::k_scheduler.start_schedule();

		while ( 1 );


		// test_noreturn();
		//pm::k_pm.vectortest();
		//pm::k_pm.stringtest();
		//pm::k_pm.maptest();
		//pm::k_pm.hashtest();   //  < -------------  threr are some problem in heap dealloc

		// fs::Buffer buf = fs::k_bufm.get_buffer( 0, 0 );
		// log_trace( "测试 buffer : %p", buf.debug_get_buffer_base() );
		// // fs::k_bufm.release_buffer( buf );
		// buf = fs::k_bufm.get_buffer( 0, 1024 );
		// log_trace( "测试 buffer : %p", buf.debug_get_buffer_base() );
		// // fs::k_bufm.release_buffer( buf );

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
	else	// smp not implement
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
	fs::Buffer buf = fs::k_bufm.read_sync( 1, 0 );
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
	fs::fat::Fat32Dbr* dbr = ( fs::fat::Fat32Dbr* ) p;
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
		"逻辑分区中的扇区数量:  %d\n"
		"FAT大小:               %d sectors\n",
		dbr->bpb.bytes_per_sector,
		dbr->bpb.sectors_per_cluster,
		dbr->bpb.reserved_sector_count,
		dbr->bpb.table_count,
		dbr->bpb.root_entry_count,
		dbr->bpb.media_type,
		dbr->bpb.sectors_per_track,
		dbr->bpb.head_side_count,
		dbr->bpb.hidden_sector_count,
		dbr->bpb.total_sectors_32,
		dbr->ebpb.table_size
	);

	uint32 rsv_sec = dbr->bpb.reserved_sector_count;
	uint32 tbl_siz = dbr->ebpb.table_size;
	uint32 cls_siz = dbr->bpb.sectors_per_cluster;

	buf = fs::k_bufm.read_sync( 1, dbr->bpb.reserved_sector_count );
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

	buf = fs::k_bufm.read_sync( 1, rsv_sec + tbl_siz * 2 + cls_siz * 0 );
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



	while ( 1 );


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

	uint32 bpg;		// blocks per group
	uint32 ipg;		// inodes pre group
	uint32 jni;		// journal inode number
	uint64 bs;		// block size
	uint64 ins;		// inode size
	uint64 spb;		// secotrs per block
	fs::ext4::SuperBlock * s_b = ( fs::ext4::SuperBlock * ) p;
	bpg = s_b->blocks_per_group;
	ipg = s_b->inodes_per_group;
	jni = s_b->journal_inum;
	bs = math::power( 2, s_b->log_block_size + 10 );
	ins = s_b->inode_size;
	spb = bs / dev::sata::k_sata_driver.get_sector_size();
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
		"  日志inode:            %d\n"
		"  首个未保留的inode号:  %d",
		s_b->inodes_count,
		( uint64 ) s_b->blocks_count_lo + ( ( uint64 ) s_b->blocks_count_hi << 32 ),
		bs,
		bpg,
		ipg,
		ins,
		s_b->reserved_gdt_blocks,
		s_b->magic,
		s_b->rev_level,
		s_b->minor_rev_level,
		jni,
		s_b->first_ino
	);

	fs::k_bufm.release_buffer_sync( buf );

	buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + spb * 1 );
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
	uint64 bg0_it = ( uint64 ) bgd->inode_table_lo + ( ( uint64 ) bgd->inode_table_hi << 32 );
	uint64 bg0_ibm = ( uint64 ) bgd->inode_bitmap_lo + ( ( uint64 ) bgd->inode_bitmap_hi << 32 );
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
		bg0_ibm,
		bg0_it
	);

	assert( jni / ipg == 0, "journal inode(%d) not in block group %d!", jni, bg_id );


	fs::k_bufm.release_buffer_sync( buf );

	// while ( 1 );

	// buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + spb * bg0_ibm );
	// p = ( char * ) buf.get_data_ptr();
	// log_info( "打印块组0 iNode bitmap的内容" );
	// printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	// for ( uint i = 0; i < 512; ++i )
	// {
	// 	if ( i % 0x10 == 0 )
	// 		printf( "%B%B\t", i >> 8, i );
	// 	printf( "%B ", p[ i ] );
	// 	if ( i % 0x10 == 0xF )
	// 		printf( "\n" );
	// }
	// log_info( "Block-group 0 iNode bitmap : 0x%x", *( uint64 * ) p );
	// fs::k_bufm.release_buffer_sync( buf );

	uint ji_idx = ( jni - 1 ) % ipg;
	uint ji_blk = ( ji_idx * ins ) / bs;
	buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + spb * ( bg0_it + ji_blk ) );
	p = ( char * ) buf.get_data_ptr();
	// log_info( "打印inode table的内容" );
	// printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	// for ( uint i = 0; i < 512; ++i )
	// {
	// 	if ( i % 0x10 == 0 )
	// 		printf( "%B%B\t", i >> 8, i );
	// 	printf( "%B ", p[ i ] );
	// 	if ( i % 0x10 == 0xF )
	// 		printf( "\n" );
	// }

	// while ( 1 );

	fs::ext4::InodeRecord * ji_inr = ( fs::ext4::InodeRecord * ) p;

	// printf( "inode flags:\n" );
	// for ( uint i = 0; i < bs / ins; ++i )
	// {
	// 	printf( "[%B] 0x%x\n", i, ( ji_inr + i )->inode_data.flags );
	// }

	// while ( 1 );

	ji_inr += ji_idx;
	p = ( char * ) ji_inr;
	log_info( "打印journal inode的内容" );
	printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	for ( uint i = 0; i < 256; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}
	if ( ji_inr->inode_data.flags & 0x80000U )	// if use extent
	{
		assert( ji_inr->inode_data.block.extents.header.magic == 0xF30A,
			"inode use extent and extent header magic should be 0xF30A, but it is 0x%x here.",
			ji_inr->inode_data.block.extents.header.magic
		);
		fs::ext4::ExtentHeader * h = &ji_inr->inode_data.block.extents.header;
		log_trace(
			">> journal inode use extent\n"
			">> present extent header below:\n"
			"||                 magic : %xH\n"
			"|| valid nodes following : %d\n"
			"||   max nodes following : %d\n"
			"||     depth of the tree : %d\n"
			"||    generation(unused) : %xH\n",
			h->magic,
			h->valid_nodes_count,
			h->max_nodes_count,
			h->depth,
			h->generation
		);

		fs::ext4::ExtentLeafNode * leaf0 = &ji_inr->inode_data.block.extents.tree_nodes[ 0 ].leaf;
		uint64 leaf0_blk0 = ( ( uint64 ) leaf0->start_hi << 32 ) + ( uint64 ) leaf0->start_lo;
		log_trace(
			">> present journal inode extent\n"
			">> level 0 - node 0\n"
			"|| covered blocks start  : %xH\n"
			"|| covered blocks length : %d\n"
			"|| extent start from     : %xH\n",
			leaf0->logical_block_start,
			leaf0->length,
			leaf0_blk0
		);

		buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + spb * leaf0_blk0 );
		p = ( char * ) buf.get_data_ptr();
		log_info( "打印journal第0块的数据" );
		printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
		for ( uint i = 0; i < 1024; ++i )
		{
			if ( i % 0x10 == 0 )
				printf( "%B%B\t", i >> 8, i );
			printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				printf( "\n" );
		}

		fs::jbd2::JournalSuperblock * jnl_sb = ( fs::jbd2::JournalSuperblock * ) p;
		log_trace(
			">> present journal super-block\n"
			"|| 魔数             :  %xH\n"
			"|| 块类型           :  %xH (3 or 4 is valid in super-block)\n"
			"|| 当前块的事务ID   :  %xH\n"
			"|| 日志设备的块大小 :  %d Bytes\n"
			"|| 日志块总数       :  %d\n"
			"|| 第一个事务ID     :  %xH\n",
			std::byteswap( jnl_sb->header.magic ),
			std::byteswap( jnl_sb->header.blocktype ),
			std::byteswap( jnl_sb->header.sequence ),
			std::byteswap( jnl_sb->blocksize ),
			std::byteswap( jnl_sb->maxlen ),
			std::byteswap( jnl_sb->first )
		);

		fs::k_bufm.release_buffer_sync( buf );

		buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + spb * ( leaf0_blk0 + 1 ) );
		p = ( char * ) buf.get_data_ptr();
		log_info( "打印journal第1块的数据" );
		printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
		for ( uint i = 0; i < 1024; ++i )
		{
			if ( i % 0x10 == 0 )
				printf( "%B%B\t", i >> 8, i );
			printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				printf( "\n" );
		}

		while ( 1 );
	}
	else
	{
		log_trace(
			"||---- journal inode ----||\n"
			"  size :             %d Bytes\n"
			"  direct block:\n"
			"                 [0] %d\n"
			"                 [1] %d\n"
			"                 [2] %d\n"
			"                 [3] %d\n"
			"                 [4] %d\n"
			"                 [5] %d\n"
			"                 [6] %d\n"
			"                 [7] %d\n"
			"                 [8] %d\n"
			"                 [9] %d\n"
			"                [10] %d\n"
			"                [11] %d\n"
			"  single indirect:   %d\n"
			"  double indirect:   %d\n"
			"  triple indirect:   %d\n"
			"  flgs:              0x%x\n",
			( uint64 ) ji_inr->inode_data.size_lo + ( ( uint64 ) ji_inr->inode_data.size_high << 32 ),
			ji_inr->inode_data.block.blocks_ptr[ 0 ], ji_inr->inode_data.block.blocks_ptr[ 1 ], ji_inr->inode_data.block.blocks_ptr[ 2 ],
			ji_inr->inode_data.block.blocks_ptr[ 3 ], ji_inr->inode_data.block.blocks_ptr[ 4 ], ji_inr->inode_data.block.blocks_ptr[ 5 ],
			ji_inr->inode_data.block.blocks_ptr[ 6 ], ji_inr->inode_data.block.blocks_ptr[ 7 ], ji_inr->inode_data.block.blocks_ptr[ 8 ],
			ji_inr->inode_data.block.blocks_ptr[ 9 ], ji_inr->inode_data.block.blocks_ptr[ 10 ], ji_inr->inode_data.block.blocks_ptr[ 11 ],
			ji_inr->inode_data.block.blocks_ptr[ 12 ], ji_inr->inode_data.block.blocks_ptr[ 13 ], ji_inr->inode_data.block.blocks_ptr[ 14 ],
			ji_inr->inode_data.flags
		);
	}

	[[maybe_unused]] uint32 j_blk0 = ji_inr->inode_data.block.blocks_ptr[ 0 ];

	fs::k_bufm.release_buffer_sync( buf );

	buf = fs::k_bufm.read_sync( 0, part_lba[ 0 ] + spb * 358 );
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

	fs::jbd2::JournalSuperblock * jsb = ( fs::jbd2::JournalSuperblock * ) p;
	log_info( "journal super block magic is 0x%x", jsb->header.magic );

	fs::k_bufm.release_buffer_sync( buf );
}

void test_fat32()
{
	// concept 说明：
	// entry 指的是一个目录在内存中的实体，它包含一系列相关的操作函数
	// dir-info 只是一段目录的信息的拷贝

	// 获取根目录entry
	fs::fat::Fat32DirEntry *fat32_root = fs::fat::k_testcase_fs.get_root_dir();
	// 本地使用一个dir-info来保存文件的信息
	fs::fat::Fat32DirInfo test_file_finfo;

	eastl::string file_name = "test_echo";

	// 通过文件名（目录名）在一个entry中查找子entry，会保存在dir-info中
	fat32_root->find_sub_dir( file_name, test_file_finfo );

	// 可以将dir-info交由fs来代理获取对应的entry
	fs::fat::Fat32DirEntry *test_file_entry = fs::fat::k_testcase_fs.get_dir_entry( test_file_finfo );

	// entry可以直接读出其中的内容，如果是文件夹会发生警告，因为读出的内容是文件系统的内部数据结构
	test_file_entry->read_content( ( void* ) tmp_buf, sizeof( tmp_buf ), 4096 );

	// 读出来后打印一下内容，这里因为读取的是一个txt，所以可以直接打印
	tmp_buf[ sizeof( tmp_buf ) - 1 ] = 0;
	log_trace( "print content of file <%s>", file_name.c_str() );
	printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	for ( uint i = 0; i < 512; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", tmp_buf[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}
	// printf( "%s", tmp_buf );
}
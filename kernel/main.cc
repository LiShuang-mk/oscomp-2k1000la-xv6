#include "console.hh"
#include "hal/loongarch.hh"
#include "hal/qemu_ls2k.hh"
#include "hal/cpu.hh"
#include "hal/disk/mbr.hh"
#include "fs/dev/pci_driver.hh"
#include "fs/dev/ahci_controller.hh"
#include "fs/fat/fat32.hh"
#include "fs/ext4/super_block.hh"
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

		log_trace(
			"hmm初始化前跟踪空闲物理页 : %d",
			mm::k_pmm.trace_free_pages_count()
		);
		mm::k_hmm.init( "k_heap manager" );
		log_info( "hmm init" );
		log_trace(
			"hmm初始化后跟踪空闲物理页 : %d",
			mm::k_pmm.trace_free_pages_count()
		);

		const uint req_size = 4000;
		int *int_arr1 = new int[ req_size ];
		int *int_arr2 = new int[ req_size ];
		tmp = ( uint64 ) int_arr1[ 0 ];
		int_arr1[ 0 ] = 0x1234;
		log_trace(
			"测试动态内存分配 :\n"
			"allocated address = %p\n"
			"allocated size    = %d Bytes\n"
			"read data p[0]    = 0x%x\n"
			"write data p[0]   = 0x%x\n",
			int_arr1,
			req_size * sizeof( int ),
			tmp,
			int_arr1[ 0 ]
		);

		delete[] int_arr1;
		delete[] int_arr2;

		int *a = new int;
		assert( a != nullptr, "new int failed!\n" );
		tmp = *a;
		*a = 0xdead;
		log_trace(
			"test new int :\n"
			"allocated address = %p\n"
			"allocated size    = %d Bytes\n"
			"read data         = 0x%x\n"
			"write data        = 0x%x\n",
			a,
			sizeof( int ),
			tmp,
			*a
		);
		delete a;

		while ( 1 );

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

		fs::k_bufm.init( "buffer manager" );
		log_info( "bufm init" );



		// test_buffer();

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
	fs::Buffer buf = fs::k_bufm.read( 0, 0 );
	char * p = ( char * ) buf.debug_get_buffer_base();
	log_info( "打印读取到buffer的内容" );
	printf( "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	for ( uint i = 0; i < mm::pg_size; ++i )
	{
		if ( i % 0x10 == 0 )
			printf( "%B%B\t", i >> 8, i );
		printf( "%B ", p[ i ] );
		if ( i % 0x10 == 0xF )
			printf( "\n" );
	}
}
//
// Created by Li shuang ( pseudonym ) on 2024-04-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "fs/fat/fat.hh"

namespace fs
{
	namespace fat
	{

		/// @brief FAT32 fs Extended BPB 
		struct Fat32Ebpb
		{
			dword table_size;					// FAT的大小（扇区），FAT32使用
			word  flags;						// 扩展标志
			word  version_number;				// 高字节是主版本号，低字节是次版本号
			dword root_cluster_number;			// 根目录簇号
			word  fsinfo_sector_number;			// 文件系统信息结构扇区号
			word  backup_boot;					// 备份引导扇区号
			byte  reserved0[ 12 ];				// 保留
			byte  drive_number;					// 物理驱动器号，硬盘通常是 0x80 开始的数字
			byte  reserve1;						// 为 Windows NT 保留的标志位
			byte  signature;					// 固定的数字签名，必须是 0x28 或 0x29 
			dword serial_number;				// 分区的序列号，可以忽略
			char  volume_label[ 11 ];			// 卷标 
			char  system_id[ 8 ];				// 文件系统 ID，通常是"FAT32 "
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct Fat32Ebpb ) == 0x5A - 0x24 );

		/// @brief FAT32 fs Dos Boot Record 
		struct Fat32Dbr
		{
			struct FatBpb bpb;
			struct Fat32Ebpb ebpb;
			byte  boot_code[ 420 ];				// 引导代码
			word  bootable_signature;			// 可引导分区签名，0xAA55
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct Fat32Dbr ) == 0x200 );

		struct Fat32Fsinfo
		{
			dword lead_signature;				// 开头签名必须是 0x41615252
			byte  reserved0[ 480 ];				//
			dword another_signature;			// 另一个签名必须是 0x61417272
			dword free_cluster_count;			// 分卷上可能的可用簇数量
			dword available_cluster_number;		// 搜索可用簇的提示簇号，即下一个可用簇被提示为在此之后出现
			byte  reserved1[ 12 ];				//
			dword trail_signature;				// 尾部签名必须是0xAA550000
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct Fat32Fsinfo ) == 0x200 );

		constexpr uint32 Fat32_fat_end_value = 0x0FFFFFF8U;
		constexpr bool fat_is_end( uint32 fat ) { return fat >= Fat32_fat_end_value; };

		constexpr uint32 Fat32_first_data_cluster = 2;


		enum Fat32DirectryAttribute : uint8
		{
			fat32_readonly = 0x01,
			fat32_hidden = 0x02,
			fat32_system = 0x04,
			fat32_volumeid = 0x08,
			fat32_directry = 0x10,
			fat32_archive = 0x20,
			fat32_longfilename = fat32_readonly | fat32_hidden | fat32_system | fat32_volumeid,
		};

		struct Fat32DirectryTimeFormat
		{
			uint16 hour : 5;
			uint16 minute : 6;
			uint16 second : 5;
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( Fat32DirectryTimeFormat ) == 2 );
		// hour: 15-11 bit | minute: 10-5 bit | second/2: 4-0 bit
		union Fat32DirectryTime { uint16 stamp; Fat32DirectryTimeFormat time; };

		struct Fat32DirectryDateFormat
		{
			uint16 year : 7;
			uint16 month : 4;
			uint16 day : 5;
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( Fat32DirectryDateFormat ) == 2 );
		// year-1980: 15-8 bit | month: 7-4 bit | day: 4-0 bit
		union Fat32DirectryDate { uint16 stamp; Fat32DirectryDateFormat time; };

		struct Fat32DirectryShort
		{
			struct
			{
				char file_name[ 8 ];
				char extension[ 3 ];
			}__attribute__( ( __packed__ ) ) file_ext;

			Fat32DirectryAttribute attribute;
			char _reserved;
			uint8 create_time_10ms;						// value from 0 to 199

			Fat32DirectryTime create_time;
			Fat32DirectryDate create_date;
			Fat32DirectryDate last_access_date;

			uint16 first_cluster_high;

			Fat32DirectryTime last_modify_time;
			Fat32DirectryDate last_modify_date;

			uint16 first_cluster_low;

			uint32 file_size_bytes;
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( Fat32DirectryShort ) == 32 );
		using Fat32DirInfo = Fat32DirectryShort;

		struct Fat32DirectryLong
		{
			using unicode_t = uint16;
			struct
			{
				uint8 seq : 5;
				uint8 _resv0 : 1;
				uint8 is_last : 1;
				uint8 _resv1 : 1;
			}__attribute__( ( __packed__ ) )
				entry_seq;

			unicode_t file_name_1[ 5 ];

			Fat32DirectryAttribute attribute;			// 0x0F

			char _reserve;
			uint8 checksum;

			unicode_t file_name_2[ 6 ];

			uint16 file_start_cluster;

			unicode_t file_name_3[ 2 ];
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( Fat32DirectryLong ) == 32 );

		inline uchar Fat32DirShortChkSum( uchar * short_file_name )
		{
			int fname_len;
			uchar Sum;
			Sum = 0;
			for ( fname_len = 11; fname_len != 0; fname_len-- )
			{
				// NOTE: The operation is an unsigned char rotate right
				Sum = ( ( Sum & 1 ) ? 0x80U : 0 ) + ( Sum >> 1 ) + *short_file_name++;
			}
			return ( Sum );
		}

	} // namespace fat

} // namespace fs

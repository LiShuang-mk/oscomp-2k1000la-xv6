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

		struct Fsinfo
		{
			dword lead_signature;				// 开头签名必须是 0x41615252
			byte  reserved0[ 480 ];				//
			dword another_signature;			// 另一个签名必须是 0x61417272
			dword free_cluster_count;			// 分卷上可能的可用簇数量
			dword available_cluster_number;		// 搜索可用簇的提示簇号，即下一个可用簇被提示为在此之后出现
			byte  reserved1[ 12 ];				//
			dword trail_signature;				// 尾部签名必须是0xAA550000
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct Fsinfo ) == 0x200 );

		class Fat32Fs
		{
		private:
			Fat32Dbr _super_block;

		public:
			
		};

	} // namespace fat

} // namespace fs

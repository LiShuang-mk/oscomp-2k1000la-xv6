//
// Created by Li shuang ( pseudonym ) on 2024-04-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace fs
{
	namespace fat
	{
		using byte = uint8;
		using word = uint16;
		using dword = uint32;

		/// @brief FAT fs BIOS Parameter Block
		struct FatBpb
		{
			byte  bootjmp[ 3 ];
			char  oem_name[ 8 ];
			word  bytes_per_sector;				// 扇区大小（字节）
			byte  sectors_per_cluster; 			// 簇大小（扇区）
			word  reserved_sector_count;		// 保留扇区数
			byte  table_count;					// FAT 数量
			word  root_entry_count;				// 根目录条目数量
			word  total_sectors_16;				// 逻辑分区中的扇区数量
			byte  media_type;					// 硬盘介质类型
			word  table_size;					// FAT 的大小（扇区），只有FAT12/16使用
			word  sectors_per_track;			// 一个磁道的扇区数
			word  head_side_count;				// 磁头的数量
			dword hidden_sector_count;			// 隐藏扇区数量
			dword total_sectors_32;				// 逻辑分区中的扇区数量，FAT32使用
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( struct FatBpb ) == 0x24 );

	} // namespace fat

} // namespace fs

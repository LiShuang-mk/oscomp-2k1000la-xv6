//
// Created by Li shuang ( pseudonym ) on 2024-04-22
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

namespace hsai
{
	/// @brief Disk Partition Table Entry
	struct DiskPartTableEntry
	{
		uint32 drive_attribute : 8;
		uint32 chs_addr_start  : 24;
		uint32 part_type	   : 8;
		uint32 chs_addr_last   : 24;
		uint32 lba_addr_start;
		uint32 sector_count;
	} __attribute__( ( __packed__ ) );
	static_assert( sizeof( DiskPartTableEntry ) == 16 );

	enum MbrPartTypeEnum : uint32
	{
		mbr_part_empty		   = 0x00,
		mbr_part_fat32_chs_dos = 0x0B,
		mbr_part_fat32_lba_dos = 0x0C,
		mbr_part_fat32_windows = 0x12,
		mbr_part_ext		   = 0x83
	};

	/// @brief Master Boot Record
	struct Mbr
	{
		uint8			   boot_code[440];
		uint32			   disk_id;
		uint16			   read_only_signature; // if equal 0x5A5A, disk is read-only.
		DiskPartTableEntry partition_table[4];
		uint16			   signature; // 0xAA55
	} __attribute__( ( __packed__ ) );
	static_assert( sizeof( Mbr ) == 0x200 );


} // namespace hsai

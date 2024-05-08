//
// Created by Li Shuang ( pseudonym ) on 2024-05-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "types.hh"

namespace fs
{
	namespace ext4
	{
		struct BlockGroupDesc
		{
			uint32 block_bitmap_lo;			/* Blocks bitmap block */
			uint32 inode_bitmap_lo;			/* Inodes bitmap block */
			uint32 inode_table_lo;			/* Inodes table block */
			uint16 free_blocks_count_lo;	/* Free blocks count */
			uint16 free_inodes_count_lo;	/* Free inodes count */
			uint16 used_dirs_count_lo;		/* Directories count */
			uint16 flags;					/* EXT4_BG_flags (INODE_UNINIT, etc) */
			uint32 exclude_bitmap_lo;		/* Exclude bitmap for snapshots */
			uint16 block_bitmap_csum_lo;	/* crc32c(s_uuid+grp_num+bbitmap) LE */
			uint16 inode_bitmap_csum_lo;	/* crc32c(s_uuid+grp_num+ibitmap) LE */
			uint16 itable_unused_lo;		/* Unused inodes count */
			uint16 checksum;				/* crc16(sb_uuid+group+desc) */
			uint32 block_bitmap_hi;			/* Blocks bitmap block MSB */
			uint32 inode_bitmap_hi;			/* Inodes bitmap block MSB */
			uint32 inode_table_hi;			/* Inodes table block MSB */
			uint16 free_blocks_count_hi;	/* Free blocks count MSB */
			uint16 free_inodes_count_hi;	/* Free inodes count MSB */
			uint16 used_dirs_count_hi;		/* Directories count MSB */
			uint16 itable_unused_hi;		/* Unused inodes count MSB */
			uint32 exclude_bitmap_hi;		/* Exclude bitmap block MSB */
			uint16 block_bitmap_csum_hi;	/* crc32c(s_uuid+grp_num+bbitmap) BE */
			uint16 inode_bitmap_csum_hi;	/* crc32c(s_uuid+grp_num+ibitmap) BE */
			uint32 reserved;
		};
	} // namespace ext4

} // namespace fs

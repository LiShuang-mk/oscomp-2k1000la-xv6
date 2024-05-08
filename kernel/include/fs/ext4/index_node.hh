//
// Created by Li Shuang ( pseudonym ) on 2024-05-09 
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
		constexpr uint32 defalut_inode_record_size = 256/*bytes*/;

		constexpr uint32 dirt_blocks_cnt = 12U;						// direct blocks count
		constexpr uint32 sind_blocks_idx = dirt_blocks_cnt;			// single indirect blocks index
		constexpr uint32 dind_blocks_idx = sind_blocks_idx + 1;		// double indirect blocks index
		constexpr uint32 tind_blocks_idx = dind_blocks_idx + 1;		// triple indirect blocks index
		constexpr uint32 inode_block_ptr_cnt = tind_blocks_idx + 1;

		struct Inode
		{
			uint16	i_mode;		/* File mode */
			uint16	i_uid;		/* Low 16 bits of Owner Uid */
			uint32	i_size_lo;	/* Size in bytes */
			uint32	i_atime;	/* Access time */
			uint32	i_ctime;	/* Inode Change time */
			uint32	i_mtime;	/* Modification time */
			uint32	i_dtime;	/* Deletion Time */
			uint16	i_gid;		/* Low 16 bits of Group Id */
			uint16	i_links_count;	/* Links count */
			uint32	i_blocks_lo;	/* Blocks count */
			uint32	i_flags;	/* File flags */

			/* OS dependent 1 */
			uint32  l_i_version;

			uint32	i_block[ inode_block_ptr_cnt ];/* Pointers to blocks */
			uint32	i_generation;	/* File version (for NFS) */
			uint32	i_file_acl_lo;	/* File ACL */
			uint32	i_size_high;
			uint32	i_obso_faddr;	/* Obsoleted fragment address */

			/* OS dependent 2 */
			uint16	l_i_blocks_high; /* were l_i_reserved1 */
			uint16	l_i_file_acl_high;
			uint16	l_i_uid_high;	/* these 2 fields */
			uint16	l_i_gid_high;	/* were reserved2[0] */
			uint16	l_i_checksum_lo;/* crc32c(uuid+inum+inode) LE */
			uint16	l_i_reserved;

			uint16	i_extra_isize;
			uint16	i_checksum_hi;	/* crc32c(uuid+inum+inode) BE */
			uint32  i_ctime_extra;  /* extra Change time      (nsec << 2 | epoch) */
			uint32  i_mtime_extra;  /* extra Modification time(nsec << 2 | epoch) */
			uint32  i_atime_extra;  /* extra Access time      (nsec << 2 | epoch) */
			uint32  i_crtime;       /* File Creation time */
			uint32  i_crtime_extra; /* extra FileCreationtime (nsec << 2 | epoch) */
			uint32  i_version_hi;	/* high 32 bits for 64-bit version */
			uint32	i_projid;		/* Project ID */
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( Inode ) == 0xA0/*160 Bytes*/,
			"Current version's Inode size is 160 bytes, but Inode structure size isn't." );

		struct InodeRecord
		{
			Inode inode_data;
			uint8 _reserved_[ defalut_inode_record_size - sizeof( Inode ) ];
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( InodeRecord ) == 256,
			"An inode-record size is 256 Bytes in ext4!");

	} // namespace ext4

} // namespace fs

//
// Created by Li Shuang ( pseudonym ) on 2024-07-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#ifndef ATTR_PACK
#define ATTR_PACK __attribute__((__packed__))
#endif

namespace fs
{
	namespace ext4
	{
		struct Ext4_4kBlock
		{
			u8 data[ 4 * _1K ];
		}ATTR_PACK;

		struct Ext4SuperBlock
		{
			// 00h
			uint32	inodes_count;					/* Inodes count */
			uint32	blocks_count_lo;				/* Blocks count */
			uint32	r_blocks_count_lo;				/* Reserved blocks count */
			uint32	free_blocks_count_lo;			/* Free blocks count */
			// 10h
			uint32	free_inodes_count;				/* Free inodes count */
			uint32	first_data_block;				/* First Data Block */
			uint32	log_block_size;					/* Block size */
			uint32	log_cluster_size;				/* Allocation cluster size */
			// 20h
			uint32	blocks_per_group;				/* # Blocks per group */
			uint32	clusters_per_group;				/* # Clusters per group */
			uint32	inodes_per_group;				/* # Inodes per group */
			uint32	mtime;							/* Mount time */
			// 30h
			uint32	wtime;							/* Write time */
			uint16	mnt_count;						/* Mount count */
			uint16	max_mnt_count;					/* Maximal mount count */
			uint16	magic;							/* Magic signature */
			uint16	state;							/* File system state */
			uint16	errors;							/* Behaviour when detecting errors */
			uint16	minor_rev_level;				/* minor revision level */
			// 40h
			uint32	lastcheck;						/* time of last check */
			uint32	checkinterval;					/* max. time between checks */
			uint32	creator_os;						/* OS */
			uint32	rev_level;						/* Revision level */
			// 50h
			uint16	def_resuid;						/* Default uid for reserved blocks */
			uint16	def_resgid;						/* Default gid for reserved blocks */
			/*
			* These fields are for EXT4_DYNAMIC_REV superblocks only.
			*
			* Note: the difference between the compatible feature set and
			* the incompatible feature set is that if there is a bit set
			* in the incompatible feature set that the kernel doesn't
			* know about, it should refuse to mount the filesystem.
			*
			* e2fsck's requirements are more strict; if it doesn't know
			* about a feature in either the compatible or incompatible
			* feature set, it must abort and not try to meddle with
			* things it doesn't understand...
			*/
			uint32	first_ino;						/* First non-reserved inode */
			uint16	inode_size;						/* size of inode structure */
			uint16	block_group_nr;					/* block group # of this superblock */
			union
			{
				u32 value;
				struct
				{
					u32 dir_prealloc : 1;
					u32 imagic_inodes : 1;
					u32 has_journal : 1;
					u32 ext_attr : 1;
					u32 resize_inode : 1;
					u32 dir_index : 1;
					u32 lazy_bg : 1;
					u32 exclude_inode : 1;
					u32 exclude_bitmap : 1;
					u32 sparse_super2 : 1;
					u32 fast_commit : 1;
					u32 orphan_present : 1;
					u32 _not_used : 20;
				}ATTR_PACK flags;
			}ATTR_PACK	feature_compat;				/* compatible feature set */
			// 60h
			union
			{
				u32 value;
				struct
				{
					u32 compression : 1;
					u32 filetype : 1;
					u32 recover : 1;
					u32 journal_dev : 1;
					u32 meta_bd : 1;
					u32 _not_used0 : 1;
					u32 extents : 1;
					u32 enable_64bit : 1;
					u32 mmp : 1;
					u32 flex_bg : 1;
					u32 ea_inode : 1;
					u32 _not_used1 : 1;
					u32 dirdata : 1;
					u32 csum_seed : 1;
					u32 largedir : 1;
					u32 inline_data : 1;
					u32 encrypt : 1;
					u32 _not_used2 : 15;
				}ATTR_PACK flags;
			}ATTR_PACK	feature_incompat;			/* incompatible feature set */
			// 64h
			union
			{
				u32 value;
				struct
				{
					u32 sparse_super : 1;
					u32 large_file : 1;
					u32 btree_dir : 1;
					u32 huge_file : 1;
					u32 gdt_csum : 1;
					u32 dir_nlink : 1;
					u32 extra_isize : 1;
					u32 has_snapshot : 1;
					u32 quota : 1;
					u32 bigalloc : 1;
					u32 metadata_csum : 1;
					u32 replica : 1;
					u32 readonly : 1;
					u32 project : 1;
					u32 _not_used0 : 1;
					u32 verity : 1;
					u32 orphan_present : 1;
					u32 _not_used1 : 15;
				}ATTR_PACK flags;
			}ATTR_PACK	feature_ro_compat;			/* readonly-compatible feature set */
			// 68h
			uint8	uuid[ 16 ];						/* 128-bit uuid for volume */
			// 78h
			char	volume_name[ 16 ];				/* volume name */
			// 88h
			char	last_mounted[ 64 ];				/* directory where last mounted */
			// C8h 
			uint32	algorithm_usage_bitmap; /* For compression */
			/*
			 * Performance hints.  Directory preallocation should only
			 * happen if the EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on.
			 */
			uint8	prealloc_blocks;				/* Nr of blocks to try to preallocate*/
			uint8	prealloc_dir_blocks;			/* Nr to preallocate for dirs */
			uint16	reserved_gdt_blocks;			/* Per group desc for online growth */
			/*
			* Journaling support valid if EXT4_FEATURE_COMPAT_HAS_JOURNAL set.
			*/
			// D0h 

			uint8	journal_uuid[ 16 ];				/* uuid of journal superblock */

			//E0h

			uint32	journal_inum;					/* inode number of journal file */
			uint32	journal_dev;					/* device number of journal file */
			uint32	last_orphan;					/* start of list of inodes to delete */
			uint32	hash_seed[ 4 ];					/* HTREE hash seed */
			uint8	def_hash_version;				/* Default hash version to use */
			uint8	jnl_backup_type;
			uint16  desc_size;						/* size of group descriptor */
			/*100*/
			uint32	default_mount_opts;
			uint32	first_meta_bg;					/* First metablock block group */
			uint32	mkfs_time;						/* When the filesystem was created */
			uint32	jnl_blocks[ 17 ];				/* Backup of the journal inode */
			/* 64bit support valid if EXT4_FEATURE_INCOMPAT_64BIT */
			/*150*/
			uint32	blocks_count_hi;				/* Blocks count */
			uint32	r_blocks_count_hi;				/* Reserved blocks count */
			uint32	free_blocks_count_hi;			/* Free blocks count */
			uint16	min_extra_isize;				/* All inodes have at least # bytes */
			uint16	want_extra_isize; 				/* New inodes should reserve # bytes */
			uint32	flags;							/* Miscellaneous flags */
			uint16  raid_stride;					/* RAID stride */
			uint16  mmp_update_interval;  			/* # seconds to wait in MMP checking */
			uint64  mmp_block;            			/* Block for multi-mount protection */
			uint32  raid_stripe_width;    			/* blocks on all data disks (N*stride)*/
			uint8	log_groups_per_flex;  			/* FLEX_BG group size */
			uint8	checksum_type;					/* metadata checksum algorithm used */
			uint8	encryption_level;				/* versioning level for encryption */
			uint8	reserved_pad;					/* Padding to next 32bits */
			uint64	kbytes_written;					/* nr of lifetime kilobytes written */
			uint32	snapshot_inum;					/* Inode number of active snapshot */
			uint32	snapshot_id;					/* sequential ID of active snapshot */
			uint64	snapshot_r_blocks_count; 		/* reserved blocks for active
														snapshot's future use */
			uint32	snapshot_list;					/* inode number of the head of the
														on-disk snapshot list */
// #define EXT4_S_ERR_START offsetof(struct ext4_super_block, s_error_count)
			uint32	error_count;					/* number of fs errors */
			uint32	first_error_time;				/* first time an error happened */
			uint32	first_error_ino;				/* inode involved in first error */
			uint64	first_error_block;				/* block involved of first error */
			uint8	first_error_func[ 32 ];			/* function where the error happened */
			uint32	first_error_line;				/* line number where error happened */
			uint32	last_error_time;				/* most recent time of an error */
			uint32	last_error_ino;					/* inode involved in last error */
			uint32	last_error_line;				/* line number where error happened */
			uint64	last_error_block;				/* block involved of last error */
			uint8	last_error_func[ 32 ];			/* function where the error happened */
// #define EXT4_S_ERR_END offsetof(struct ext4_super_block, s_mount_opts)
			uint8	mount_opts[ 64 ];
			uint32	usr_quota_inum;					/* inode for tracking user quota */
			uint32	grp_quota_inum;					/* inode for tracking group quota */
			uint32	overhead_clusters;				/* overhead blocks/clusters in fs */
			uint32	backup_bgs[ 2 ];				/* groups with sparse_super2 SBs */
			uint8	encrypt_algos[ 4 ];				/* Encryption algorithms in use  */
			uint8	encrypt_pw_salt[ 16 ];			/* Salt used for string2key algorithm */
			uint32	lpf_ino;						/* Location of the lost+found inode */
			uint32	prj_quota_inum;					/* inode for tracking project quota */
			uint32	checksum_seed;					/* crc32c(uuid) if csum_seed set */
			uint8	wtime_hi;
			uint8	mtime_hi;
			uint8	mkfs_time_hi;
			uint8	lastcheck_hi;
			uint8	first_error_time_hi;
			uint8	last_error_time_hi;
			uint8	first_error_errcode;
			uint8   last_error_errcode;
			uint16  encoding;						/* Filename charset encoding */
			uint16  encoding_flags;					/* Filename charset encoding flags */
			uint32  orphan_file_inum;				/* Inode for tracking orphan inodes */
			uint32	reserved[ 94 ];					/* Padding to the end of the block */
			uint32	checksum;						/* crc32c(superblock) */
		}ATTR_PACK;
		static_assert( sizeof( Ext4SuperBlock ) == 0x400 );

		struct Ext4GroupDesc
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
		}ATTR_PACK;
		static_assert( sizeof( Ext4GroupDesc ) == 64 );


		constexpr uint32 defalut_inode_record_size = 256/*bytes*/;
		constexpr uint32 dirt_blocks_cnt = 12U;						// direct blocks count
		constexpr uint32 sind_blocks_idx = dirt_blocks_cnt;			// single indirect blocks index
		constexpr uint32 dind_blocks_idx = sind_blocks_idx + 1;		// double indirect blocks index
		constexpr uint32 tind_blocks_idx = dind_blocks_idx + 1;		// triple indirect blocks index
		constexpr uint32 inode_block_ptr_cnt = tind_blocks_idx + 1;

		struct Ext4ExtentHeader
		{
			uint16 magic;
			uint16 valid_nodes_count;		// number of valid nodes followed by this header
			uint16 max_nodes_count;			// max number of nodes followed by this header
			uint16 depth;					// the depth of this extents tree
			uint32 generation;				// not in standard EXT4
		}ATTR_PACK;
		static_assert( sizeof( Ext4ExtentHeader ) == 12 );

		struct Ext4ExtentInternalNode
		{
			uint32 logical_block_start;		// the start logical block of the extent encompassed by this index node 
			uint64 next_node_address : 48;	// block addrees of the next lower level node that this node is pointing to
			uint64 _unused : 16;
		}ATTR_PACK;
		static_assert( sizeof( Ext4ExtentInternalNode ) == 12 );


		struct Ext4ExtentLeafNode
		{
			uint32 logical_block_start;		// the logical start block of the extent encompassed by this leaf node
			uint16 length;					// the length of this extent, max is 32768; the highest bit set means uninitialized
			uint16 start_hi;				// the physical start block high address of this extent 
			uint32 start_lo;				// similar to above but low address
		}ATTR_PACK;
		static_assert( sizeof( Ext4ExtentLeafNode ) == 12 );

		struct Ext4Inode
		{
		// 0x000

			uint16 mode;			/* File mode */
			uint16 uid;				/* Low 16 bits of Owner Uid */
			uint32 size_lo;			/* Size in bytes */
			uint32 atime;			/* Access time */
			uint32 ctime;			/* Inode Change time */

		// 0x010

			uint32 mtime;			/* Modification time */
			uint32 dtime;			/* Deletion Time */
			uint16 gid;				/* Low 16 bits of Group Id */
			uint16 links_count;		/* Links count */
			uint32 blocks_lo;		/* Blocks count */

		// 0x020

			uint32 flags;			/* File flags */

			/* OS dependent 1 */
			uint32 version;

		// 0x028

			union _block_u_t_
			{
				uint32 blocks_ptr[ inode_block_ptr_cnt ];
				struct _extent_s_t_
				{
					Ext4ExtentHeader header;
					union _node_u_t_
					{
						Ext4ExtentInternalNode internal;
						Ext4ExtentLeafNode     leaf;
					}ATTR_PACK tree_nodes[ 4 ];
				}ATTR_PACK extents;
			}ATTR_PACK blocks;						/* Pointers to blocks */
			static_assert( sizeof( _block_u_t_ ) == 60/*Bytes*/ );

		// 0x064

			uint32 generation;							/* File version (for NFS) */
			uint32 file_acl_lo;							/* File ACL */
			uint32 size_high;

		// 0x070

			uint32 obso_faddr;							/* Obsoleted fragment address */

			/* OS dependent 2 */
			uint16 blocks_high; 						/* were l_i_reserved1 */
			uint16 file_acl_high;
			uint16 uid_high;							/* these 2 fields */
			uint16 gid_high;							/* were reserved2[0] */
			uint16 checksum_lo;							/* crc32c(uuid+inum+inode) LE */
			uint16 reserved;

		// 0x080

			uint16 extra_isize;
			uint16 checksum_hi;							/* crc32c(uuid+inum+inode) BE */
			uint32 ctime_extra;							/* extra Change time      (nsec << 2 | epoch) */
			uint32 mtime_extra;							/* extra Modification time(nsec << 2 | epoch) */
			uint32 atime_extra;							/* extra Access time      (nsec << 2 | epoch) */

		// 0x090

			uint32 crtime;								/* File Creation time */
			uint32 crtime_extra; 						/* extra FileCreationtime (nsec << 2 | epoch) */
			uint32 version_hi;							/* high 32 bits for 64-bit version */
			uint32 projid;								/* Project ID */

		// 0x0A0
		}ATTR_PACK;
		static_assert( sizeof( Ext4Inode ) == 0xA0/*160 Bytes*/,
			"Current version's Inode size is 160 bytes, but Inode structure size isn't." );

		struct Ext4InodeRecord
		{
			Ext4Inode inode_data;
			uint8 _reserved_[ defalut_inode_record_size - sizeof( Ext4Inode ) ];
		}ATTR_PACK;
		static_assert( sizeof( Ext4InodeRecord ) == 256,
			"An inode-record size is 256 Bytes in ext4!" );


		constexpr int dirent_max_name_len = 255;
		struct Ext4DirEntry
		{
			u32 inode;
			u16 rec_len;
			u16 name_len;
			char name[ dirent_max_name_len];
		}ATTR_PACK;

		struct Ext4DirEntryHash
		{
			u32 hash;
			u32 minor_hash;
		}ATTR_PACK;

		struct Ext4DirEntry2
		{
			u32 inode;
			u16 rec_len;
			u8  name_len;
			u8  file_type;
			char name[ dirent_max_name_len ];
		}ATTR_PACK;
		static_assert( sizeof( Ext4DirEntry ) == sizeof( Ext4DirEntry2 ) );

		struct Ext4DirEntryTail
		{
			u32 _rsv_inode;				// should be 0
			u16 rec_len;
			u8  _rsv_namelen;			// should be 0
			u8  _rsv_filetype;			// should be 0xDE
			u32 checksum;
		}ATTR_PACK;
		static_assert( sizeof( Ext4DirEntryTail ) == 12 );





	} // namespace ext4

} // namespace fs

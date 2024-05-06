//
// Created by Li shuang ( pseudonym ) on 2024-04-22 
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
		struct SuperBlock
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
			uint32	feature_compat;					/* compatible feature set */
			// 60h
			uint32	feature_incompat;				/* incompatible feature set */
			uint32	feature_ro_compat;				/* readonly-compatible feature set */
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
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( SuperBlock ) == 0x400 );

	} // namespace ext

} // namespace fs

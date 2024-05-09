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

		struct ExtentHeader
		{
			uint16 magic;
			uint16 valid_nodes_count;		// number of valid nodes followed by this header
			uint16 max_nodes_count;			// max number of nodes followed by this header
			uint16 depth;					// the depth of this extents tree
			uint32 generation;				// not in standard EXT4
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( ExtentHeader ) == 12 );

		struct ExtentInternalNode
		{
			uint32 logical_block_start;		// the start logical block of the extent encompassed by this index node 
			uint64 next_node_address : 48;	// block addrees of the next lower level node that this node is pointing to
			uint64 _unused : 16;
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( ExtentInternalNode ) == 12 );


		struct ExtentLeafNode
		{
			uint32 logical_block_start;		// the logical start block of the extent encompassed by this leaf node
			uint16 length;					// the length of this extent, max is 32768; the highest bit set means uninitialized
			uint16 start_hi;				// the physical start block high address of this extent 
			uint32 start_lo;				// similar to above but low address
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( ExtentLeafNode ) == 12 );

		static_assert( sizeof( ExtentInternalNode ) == sizeof( ExtentLeafNode ) );

		struct Inode
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
					ExtentHeader header;
					union _node_u_t_
					{
						ExtentInternalNode internal;
						ExtentLeafNode     leaf;
					}__attribute__( ( __packed__ ) )
						tree_nodes[
							( inode_block_ptr_cnt * sizeof( uint32 ) - sizeof( header ) ) / sizeof( ExtentLeafNode )
						];
					static_assert( inode_block_ptr_cnt * sizeof( uint32 ) % sizeof( ExtentLeafNode ) == 0 );

				}__attribute__( ( __packed__ ) )
					extents;

			}__attribute__( ( __packed__ ) )
				block;						/* Pointers to blocks */

				// uint32 block[ inode_block_ptr_cnt ];	

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
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( Inode ) == 0xA0/*160 Bytes*/,
			"Current version's Inode size is 160 bytes, but Inode structure size isn't." );

		struct InodeRecord
		{
			Inode inode_data;
			uint8 _reserved_[ defalut_inode_record_size - sizeof( Inode ) ];
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( InodeRecord ) == 256,
			"An inode-record size is 256 Bytes in ext4!" );

	} // namespace ext4

} // namespace fs

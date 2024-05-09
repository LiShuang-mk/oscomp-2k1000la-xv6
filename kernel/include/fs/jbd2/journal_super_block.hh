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
	namespace jbd2
	{
		struct JournalSuperblock
		{
		/* 0x0000 */

			struct
			{
				uint32 magic;
				uint32 blocktype;
				uint32 sequence;
			} header;

		/* 0x000C */
			/* Static information describing the journal */
			uint32	blocksize;		/* journal device blocksize */
			uint32	maxlen;			/* total blocks in journal file */
			uint32	first;			/* first block of log information */

		/* 0x0018 */
			/* Dynamic information describing the current state of the log */
			uint32	sequence;		/* first commit ID expected in log */
			uint32	start;			/* blocknr of start of log */

		/* 0x0020 */
			/* Error value, as set by jbd2_journal_abort(). */
			uint32	errno;

		/* 0x0024 */
			/* Remaining fields are only valid in a version-2 superblock */
			uint32	feature_compat;		/* compatible feature set */
			uint32	feature_incompat;	/* incompatible feature set */
			uint32	feature_ro_compat;	/* readonly-compatible feature set */

		/* 0x0030 */

			uint8	uuid[ 16 ];			/* 128-bit uuid for journal */

		/* 0x0040 */

			uint32	nr_users;			/* Nr of filesystems sharing log */
			uint32	dynsuper;			/* Blocknr of dynamic superblock copy*/

		/* 0x0048 */

			uint32	max_transaction;	/* Limit of journal blocks per trans.*/
			uint32	max_trans_data;		/* Limit of data blocks per trans. */

		/* 0x0050 */

			uint8	checksum_type;		/* checksum type */
			uint8	padding2[ 3 ];

		/* 0x0054 */

			uint32	_num_fc_blks;		/* Number of fast commit blocks */
			uint32	_head;				/* blocknr of head of log, only uptodate
										 * while the filesystem is clean */
		/* 0x005C */

			uint32	padding[ 40 ];
			uint32	checksum;			/* crc32c(superblock) */

		/* 0x0100 */

			uint8	users[ 16 * 48 ];	/* ids of all fs'es sharing the log */

		/* 0x0400 */
		}__attribute__( ( __packed__ ) );

		static_assert( sizeof( JournalSuperblock ) == 0x400, "journal super-block size should be 1024 Bytes" );

	} // namespace jbd2

} // namespace fs

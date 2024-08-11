//
// Created by Li Shuang ( pseudonym ) on 2024-07-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "fs/ext4/ext4.hh"
#include "fs/inode.hh"

namespace fs
{
	namespace ext4
	{
		class Ext4FS;
		class Ext4Buffer;
		class Ext4IndexNode : public Inode
		{
		private:
			Ext4Inode _inode;
			Ext4FS * _belong_fs = nullptr;
			long _has_blocks = 0;			// 通常是指 4KiB block
			long _has_size = 0;				// bytes
			FileAttrs _attrs;
		public:
			Ext4IndexNode() = default;
			Ext4IndexNode( Ext4Inode &inode, Ext4FS * fs );
			virtual ~Ext4IndexNode() override = default;

			virtual Inode *lookup( eastl::string dirname ) override;
			virtual Inode *mknode( eastl::string name, FileAttrs attrs, eastl::string dev_name ) override { return nullptr; };

			virtual size_t nodeRead( u64 dst, size_t off, size_t len ) override;
			virtual size_t nodeWrite( u64 src, size_t off, size_t len ) override { return 0; };
			virtual int readlinkat( char *buf, size_t len ) override { return 0; };
			
		public:
			virtual FileAttrs rMode() const override { return _attrs; };
			virtual dev_t rDev() const override { return -1; };
			virtual u64 rFileSize() const override { return -1; };
			virtual u64 rIno() const override { return 0; };
			virtual SuperBlock *getSb() const override;
			virtual FileSystem *getFS() const override { return ( FileSystem * ) _belong_fs; };

		public:
			/// @brief 读取 inode 所含的某一个逻辑块
			/// @param block 对于当前inode而言的逻辑块号（从0开始的块号，0号块是文件开头）
			/// @param pin pin住buffer，使其不要被释放（unpin可以释放buffer）
			/// @return 数据块的buffer
			Ext4Buffer * read_logical_block( long block, bool pin = false );

		private:
			void _cal_size();
			void _cal_blocks();

			Ext4Buffer * _search_direct_block( long target_block_no, long start_block_no, void * index_block, bool pin = false );
			Ext4Buffer * _search_sindirect_block( long target_block_no, long start_block_no, void * index_block, bool pin = false );
			Ext4Buffer * _search_dindirect_block( long target_block_no, long start_block_no, void * index_block, bool pin = false );
			Ext4Buffer * _search_tindirect_block( long target_block_no, long start_block_no, void * index_block, bool pin = false );
			long _cal_tind_index( long target_block, long start_block );
			long _cal_dind_index( long target_block, long start_block );
			long _cal_sind_index( long target_block, long start_block );

			Inode * _htree_lookup( eastl::string &dir_name );
			Inode * _linear_lookup( eastl::string &dir_name, void * block );

		public: // debug
			void debug_hash( eastl::string dir_name );

		};

	} // namespace ext4

} // namespace fs

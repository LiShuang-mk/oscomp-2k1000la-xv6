//
// Created by Li Shuang ( pseudonym ) on 2024-07-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "fs/fs_defs.hh"
#include "fs/ext4/ext4.hh"

namespace fs
{
	namespace ext4
	{
		class Ext4FS;
		class Ext4BlockGroup;

		class Ext4SB : public SuperBlock
		{
			friend Ext4FS;
			friend Ext4BlockGroup;
		private:
			Ext4SuperBlock _super_block;
			FileSystem * _belong_fs = nullptr;
			bool _support_64bit = false;
			size_t _cache_block_size = 0;
			size_t _cache_block_count = 0;
			size_t _cache_free_block_count = 0;

		public:
			Ext4SB() = default;
			Ext4SB( Ext4SuperBlock * sb, FileSystem * fs );
			virtual ~Ext4SB();
			virtual dentry * getRoot() const override { return nullptr; };
			virtual dentry * getMntPoint() const override { return nullptr; };
			virtual FileSystem * getFileSystem() const override { return _belong_fs; };
			virtual bool isValid() const override { return _cache_block_size != 0; };
			virtual u32 rDefaultMod() const override { return 0; };
			virtual size_t rBlockSize() const override { return _cache_block_size; };
			virtual size_t rBlockNum() const override { return _cache_block_count; };
			virtual Inode *allocInode( mode_t mode ) override { return nullptr; };

			size_t rBlockFree() const { return _cache_free_block_count; }

			bool support64bit() const { return _support_64bit; }
		};

	} // namespace ext4

} // namespace fs

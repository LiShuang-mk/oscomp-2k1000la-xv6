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
		class Ext4IndexNode : public Inode
		{
		private:
			Ext4Inode _inode;
			Ext4FS * _belong_fs = nullptr;
			long _has_blocks = 0;
			long _has_size = 0;

		public:
			Ext4IndexNode() = default;
			Ext4IndexNode( Ext4Inode &inode, Ext4FS * fs );
			virtual ~Ext4IndexNode() override = default;

			virtual Inode *lookup( eastl::string dirname, off_t off = 0 ) override;
			virtual Inode *mknode( eastl::string name, mode_t mode ) override { return nullptr; };

			virtual size_t nodeRead( u64 dst, size_t off, size_t len ) override { return 0; };
			virtual size_t nodeWrite( u64 src, size_t off, size_t len ) override { return 0; };

		public:
			virtual mode_t rMode() const override { return 0; };
			virtual dev_t rDev() const override { return -1; };
			virtual u64 rFileSize() const override { return -1; };
			virtual u64 rIno() const override { return 0; };
			virtual SuperBlock *getSb() const override { return nullptr; };
			virtual FileSystem *getFS() const override { return nullptr; };

		private:
			void _cal_size();
			void _cal_blocks();
		};

	} // namespace ext4
	
} // namespace fs

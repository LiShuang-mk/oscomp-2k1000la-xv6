//
// Created by Li Shuang ( pseudonym ) on 2024-07-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "fs/inode.hh"

namespace fs
{
	namespace ext4
	{
		class Ext4IndexNode : public Inode
		{
		public:
			Ext4IndexNode() = default;
			virtual ~Ext4IndexNode() override = default;

			virtual Inode *lookup( eastl::string dirname ) override { return nullptr; };
			virtual Inode *mknode( eastl::string name, mode_t mode, int dev = -1 ) override { return nullptr; };

			virtual size_t nodeRead( u64 dst, size_t off, size_t len ) override { return 0; };
			virtual size_t nodeWrite( u64 src, size_t off, size_t len ) override { return 0; };

		public:
			virtual mode_t rMode() const override { return 0; };
			virtual dev_t rDev() const override { return -1; };
			virtual u64 rFileSize() const override { return -1; };
			virtual u64 rIno() const override { return 0; };
			virtual SuperBlock *getSb() const override { return nullptr; };
			virtual FileSystem *getFS() const override { return nullptr; };
		};

	} // namespace ext4
	
} // namespace fs

//
// Created by Li Shuang ( pseudonym ) on 2024-07-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "fs/fs.hh"
#include "fs/ext4/ext4_sb.hh"

namespace fs
{
	namespace ext4
	{
		class Ext4FS : public FileSystem
		{
		private:
			bool _is_root = false;
			Ext4SB _sb;

		public:
			Ext4FS() = default;
			virtual ~Ext4FS() override = default;
			virtual void init( int dev, u64 start_lba, eastl::string fstype, eastl::string rootname, bool is_root = false ) override;
			virtual size_t rBlockSize() const override { return _sb.rBlockSize(); };
			virtual long rBlockNum() const override { return _sb.rBlockNum(); };
			virtual long rBlockFree() const override { return _sb.rBlockFree(); };
			virtual long rMaxFile() const override { return -1; };
			virtual long rFreeFile() const override { return -1; };
			virtual size_t rNamelen() const override { return -1; };
			virtual dentry *getRoot() const override { return _sb.getRoot(); };
			virtual dentry *getMntPoint() const override { return _sb.getMntPoint(); };
			virtual mode_t rDefaultMod() const override { return _sb.rDefaultMod(); };
			virtual int mount( dentry *dev, dentry *mnt, eastl::string fstype ) override { return -1; };
			virtual int umount( dentry *mnt ) override { return -1; };

		public:
			virtual bool isRootFS() const override { return _is_root; }
			virtual SuperBlock *getSuperBlock() const override { return ( SuperBlock * ) &_sb; };
			virtual eastl::string rFStype() const override { return "ext4"; }
		};

	} // namespace ext4

} // namespace fs

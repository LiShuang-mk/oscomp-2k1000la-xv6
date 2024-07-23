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
#include "fs/ext4/ext4_inode.hh"
#include "fs/ext4/ext4_block_group.hh"
#include "fs/ext4/ext4_buffer.hh"

namespace fs
{
	namespace ext4
	{
		class Ext4FS : public FileSystem
		{
			friend Ext4BlockGroup;
		private:
			bool _is_root = false;

			int _owned_dev = -1;
			ulong _start_lba = 0;
			Ext4SB _sb;
			Ext4IndexNode _root_inode;

			dentry * _root_dir = nullptr;

			int _cache_group_count = 0;
			int _cache_blocks_per_group = 0;
			int _cache_inodes_per_group = 0;
			Ext4BlockGroup * _gdt = nullptr;				// block Group Descriptor Table

			Ext4BufferPool _blocks_cacher;

		public:
			Ext4FS() = default;
			virtual ~Ext4FS() override = default;
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
			virtual bool isRootFS() const override { return _is_root; }
			virtual SuperBlock *getSuperBlock() const override { return ( SuperBlock * ) &_sb; };
			virtual eastl::string rFStype() const override { return "ext4"; }

		public:
			virtual void init( int dev, u64 start_lba, eastl::string fstype, eastl::string rootname, bool is_root = false ) override;

		public:
			bool support64bit() const { return _sb.support64bit(); }
			int owned_device() const { return _owned_dev; }
			ulong start_lba() const { return _start_lba; }
			void * read_block( long block_no ) { return _blocks_cacher.request_block( block_no )->get_data_ptr(); }

		public:
			/// @brief 从指定块号开始读取数据
			/// @param block_no 块号
			/// @param dst 保存数据的地址
			/// @param size 读取数据的长度（字节）
			void read_data( long block_no, void * dst, long size );
		};

	} // namespace ext4

} // namespace fs

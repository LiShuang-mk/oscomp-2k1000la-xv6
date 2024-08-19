//
// Created by Li Shuang ( pseudonym ) on 20240-7-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "fs/ext4/ext4.hh"

namespace fs
{
	namespace ext4
	{
		class Ext4FS;

		class Ext4BlockGroup
		{
			friend Ext4FS;

		private:

			Ext4GroupDesc _gd;						// group descriptor
			Ext4FS * _belong_fs = nullptr;
			u8 *_block_bitmap = nullptr;			// bitmap 大小等于一个块的大小，ext4中通常为4KiB
			u8 *_inode_bitmap = nullptr;			// 同上
			long _cache_inode_table_block = 0;

		public:
			Ext4BlockGroup() = default;
			Ext4BlockGroup( Ext4GroupDesc * gd, Ext4FS * fs );

			/// @brief 读取 inode 结构体
			/// @param index inode 在块组inode表中的下标
			/// @param node 读取到的inode
			/// @return 成功返回 0
			int read_inode( long index, Ext4Inode &node );

		private:
			void _read_block_bitmap();
			void _read_inode_bitmap();
		};

	} // namespace ext4
	
} // namespace fs

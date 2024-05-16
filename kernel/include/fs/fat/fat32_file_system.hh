//
// Created by Li Shuang ( pseudonym ) on 2024-05-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "fs/fat/fat32.hh"
#include "fat32_dir_entry.hh"

namespace fs
{
	namespace fat
	{
		class Fat32FileSystem
		{
		private:
			uint _device;
			uint64 _start_lba;
			Fat32Dbr _super_block;
			Fat32DirEntry _root;

		public:
			void init( uint device, uint64 part_start_lba );
			uint owned_device() { return _device; }
			uint64 start_lba() { return _start_lba; }
			Fat32Dbr * get_super_block() { return &_super_block; }
		};

	} // namespace fat

} // namespace fs

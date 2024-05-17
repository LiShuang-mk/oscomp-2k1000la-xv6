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

			Fat32DirEntry _cache_entry;

		public:
			void init( uint device, uint64 part_start_lba );

			Fat32DirEntry * get_dir_entry( Fat32DirInfo &dir_info );

		public:
			uint owned_device() { return _device; }
			uint64 start_lba() { return _start_lba; }

			Fat32Dbr * get_super_block() { return &_super_block; }
			uint64 get_reserved_sectors() { return _super_block.bpb.reserved_sector_count; }
			uint64 get_fat_counts() { return _super_block.bpb.table_count; }
			uint64 get_sectors_per_fat() { return _super_block.ebpb.table_size; }
			uint64 get_sectors_per_cluster() { return _super_block.bpb.sectors_per_cluster; }
			uint64 get_bytes_per_sector() { return _super_block.bpb.bytes_per_sector; }

			Fat32DirEntry * get_root_dir() { return &_root; }
		};

		extern Fat32FileSystem k_testcase_fs;

	} // namespace fat

} // namespace fs

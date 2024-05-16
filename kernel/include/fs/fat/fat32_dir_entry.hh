//
// Created by Li Shuang ( pseudonym ) on -- 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "fat32.hh"

#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace fs
{
	namespace fat
	{
		class Fat32FileSystem;

		enum Fat32DirEntryStatus
		{
			fat32de_undefined,
			fat32de_init_ok,
			fat32de_init_fail
		};

		class Fat32DirEntry
		{
			friend Fat32FileSystem;

		private:
			Fat32FileSystem *_belong_fs;
			eastl::vector<uint32> _clusters_number;
			eastl::string _dir_name;
			eastl::unordered_map<eastl::string, uint> _children;

		public:
			Fat32DirEntryStatus init( uint cluster_number );

		};
	} // namespace fat
	
} // namespace fs

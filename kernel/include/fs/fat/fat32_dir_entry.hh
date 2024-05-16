//
// Created by Li Shuang ( pseudonym ) on -- 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "fat32.hh"

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>

namespace fs
{
	namespace fat
	{			
		class Fat32DirEntry
		{
		private:
			eastl::vector<uint32> _clusters_number;

		public:
			int init( uint cluster_number );
		};
	} // namespace fat
	
} // namespace fs

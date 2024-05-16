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
		class Fat32Fs
		{
		private:
			Fat32Dbr _super_block;
			Fat32DirEntry _root;

		public:
			
		};

	} // namespace fat

} // namespace fs

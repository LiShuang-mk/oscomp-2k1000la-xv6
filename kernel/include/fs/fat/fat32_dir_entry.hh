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
			fat32de_init_fail,
			fat32de_find_subdir_success,
			fat32de_find_subdir_fail,
		};

		enum Fat32DirType
		{
			fat32de_unkown,
			fat32de_file,
			fat32de_folder
		};

		class Fat32DirEntry
		{
			friend Fat32FileSystem;
			using cluster_num_t = uint;
		private:
			Fat32FileSystem *_belong_fs;
			Fat32DirType _dir_type;
			eastl::vector<uint32> _clusters_number;
			eastl::unordered_map<eastl::string, Fat32DirectryShort> _children;

		public:
			Fat32DirEntry();
			
			/**************************************************
			 * TODO:
			 * [1] 此处对于目录类型是文件夹而言是有隐含的bug的，
			 * 由于buffer的大小是一个页面，如果文件夹下的内容不
			 * 能够用一个页面装下，就会发生buffer越界，从而可能
			 * 导致读取到错误的内容。实际上，如果有一个目录项（
			 * 特别是长文件名的目录项）跨越了两个buffer，那么在
			 * 调用后面的 _read_sub_dir 时就会发生越界。
			 * [tofix] 考虑将跨越两个buffer的目录项单独处理，或
			 * 是对_read_sub_dir 进行修复
			 **************************************************/
			Fat32DirEntryStatus init( uint cluster_number, Fat32DirType dir_type );


			/// @brief 在当前DirEntry中获取子Entry
			/// @param dir_name 子目录项的名字（文件名）
			/// @param dir_info 返回子目录项的信息
			/// @return 查找结果
			Fat32DirEntryStatus find_sub_dir( eastl::string &dir_name, Fat32DirInfo &dir_info );


			void read_content( void *buf, uint64 read_len, uint64 offset, bool async_read = false );

		public:
			void debug_set_belong_fs( Fat32FileSystem* fs ) { _belong_fs = fs; }


		private:
			int _read_sub_dir( void * &sub_dir_desc_ptr );
			uint64 _cluster_to_lba( uint64 cluster );
			uint64 _cover_size_bytes();

		private:

		};
	} // namespace fat

} // namespace fs

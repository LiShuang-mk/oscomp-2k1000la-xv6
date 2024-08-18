//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
namespace fs
{
	class File;
	class FileSystem;
	class dentry;
	class file;
	struct FileAttrs;
	class Path
	{
	private:
		dentry *base;
		eastl::string pathname;
		eastl::vector<eastl::string> dirname;

	public:
		Path() = default;
		Path( const Path& path ) = default;
		Path( const eastl::string& path_, file *base_ );
		Path( const eastl::string& path_, dentry *base_ )
			: base( base_ )
			, pathname( path_ )
		{
			pathbuild();
		}
		Path( const eastl::string& path_ )
			: base( nullptr )
			, pathname( path_ )
		{
			pathbuild();
		}
		Path( const char *str_, file *base_ );
		Path( const char *str_, dentry *base_ )
			: base( base_ )
			, pathname( str_ )
		{
			pathbuild();
		}
		Path( const char *str_ )
			: base( nullptr )
			, pathname( str_ )
		{
			pathbuild();
		}
		Path( file *base_ );
		Path( dentry *base_ )
			: base( base_ )
		{
			pathbuild();
		}
		~Path() = default;
		Path& operator=( const Path& path ) = default;

		void pathbuild();
		eastl::string AbsolutePath() const;
		dentry *pathHitTable();
		dentry *pathSearch( bool parent = false );
		dentry *pathCreate( uint32 mode );
		int mount( Path &dev, eastl::string fstype , uint64 flags, uint64 data);
		int umount( uint64 flags );
		int open( FileAttrs attrs, int flags );
		eastl::string rPathName() { return pathname; }
		eastl::string rFileName() { return dirname[ dirname.size() - 1 ]; }
	};

	extern eastl::unordered_map<eastl::string, FileSystem *> mnt_table;
} // namespace fs

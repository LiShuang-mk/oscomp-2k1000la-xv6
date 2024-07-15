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
	class Dentry;
	class File;
	class FileSystem;
	
	class Path
	{
	private:
		Dentry *base;
		eastl::string pathname;
		eastl::vector<eastl::string> dirname;

	public:
		Path() = default;
		Path( const Path& path ) = default;
		Path( const eastl::string& path_, File *base_ );
		Path( const eastl::string& path_, Dentry *base_ )
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
		Path( const char *str_, File *base_ );
		Path( const char *str_, Dentry *base_ )
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
		Path( File *base_ );
		Path( Dentry *base_ )
			: base( base_ )
		{
			pathbuild();
		}
		~Path() = default;
		Path& operator=( const Path& path ) = default;

		void pathbuild();
		eastl::string AbsolutePath() const;
		Dentry *pathHitTable();
		Dentry *pathSearch( bool parent = false );
		Dentry *pathCreate( uint32 mode );
		int mount( Path &dev, eastl::string fstype , uint64 flags, uint64 data);
	};

	extern eastl::unordered_map<eastl::string, FileSystem *> mnt_table;
} // namespace fs

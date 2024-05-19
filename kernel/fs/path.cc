//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/path.hh"
#include "fs/file.hh"
#include "pm/process_manager.hh"

#include <EASTL/unordered_map.h>

namespace fs
{
	eastl::unordered_map<eastl::string, FileSystem *> mnt_table;

	Path::Path( const eastl::string& path_, File *base_ )
		: base( base_ == nullptr ? nullptr : base_->data.get_Entry() )
		, pathname( path_ )
	{
		pathbuild();
	}

	Path::Path( const char *str_, File *base_ )
		: base( base_ == nullptr ? nullptr : base_->data.get_Entry() )
		, pathname( str_ )
	{
		pathbuild();
	}

	Path::Path( File *base_ )
		: base( base_->data.get_Entry() )
	{
		pathbuild();
	}

	void Path::pathbuild()
	{
		if ( pathname.size() < 1 ) { base = pm::k_pm.get_cur_pcb()->get_cwd(); return; }
		else if ( pathname[ 0 ] == '/' ) { base = mnt_table[ "/" ]->getSuperBlock()->getRoot(); }
		else if ( base == nullptr ) { base = pm::k_pm.get_cur_pcb()->get_cwd(); }

		size_t len = pathname.size();
		if ( len > 0 )
		{  // 保证数组长度不为0
			auto ind = new size_t[ len ][ 2 ] { { 0, 0 } };
			bool rep = true;
			int dirnum = 0;
			for ( size_t i = 0; i < len; ++i )
			{  // 识别以'/'结尾的目录
				if ( pathname[ i ] == '/' )
				{
					if ( !rep )
					{
						rep = true;
						++dirnum;
					}
				}
				else
				{
					++( ind[ dirnum ][ 1 ] );
					if ( rep )
					{
						rep = false;
						ind[ dirnum ][ 0 ] = i;
					}
				}
			}
			if ( !rep ) { ++dirnum; }  // 补齐末尾'/'
			dirname = eastl::vector<eastl::string>( dirnum );
			for ( size_t i = 0; i < ( size_t ) dirnum; ++i ) { dirname[ i ] = pathname.substr( ind[ i ][ 0 ], ind[ i ][ 1 ] ); }
			delete[] ind;
		}
		return;
	}

	eastl::string Path::AbsolutePath() const
	{
		eastl::vector<eastl::string> absname = dirname;
		for ( Dentry * entry = base;
			!( entry->isRoot() && entry->getNode()->getSb()->getFileSystem()->isRootFS() );
			entry = entry->getParent() )
		{
			absname.emplace( absname.begin(), entry->getName() );
		}
		eastl::string path_abs = "";
		for ( auto name = absname.begin(); name != absname.end();)
		{
			if ( *name == "." || *name == ".." ) { absname.erase( name ); }
			else if ( next( name ) != absname.end() && *( next( name ) ) == ".." ) { name = absname.erase( name ); }
			else
			{
				path_abs += "/" + *name;
				name++;
			}
		}
		if ( path_abs == "" ) { path_abs = "/"; }

		return path_abs;
	}

	Dentry * Path::pathHitTable()
	{
		eastl::string path_abs = AbsolutePath();
		size_t len = path_abs.size();
		size_t longest = 0;
		eastl::string longest_prefix = "";
		for ( auto tbl : mnt_table )
		{
			eastl::string prefix = tbl.first;
			size_t len_prefix = prefix.size();
			if ( len >= len_prefix && len_prefix > longest && path_abs.compare( 0, len_prefix, prefix ) == 0 )
			{
				longest = len_prefix;
				longest_prefix = prefix;
			}
		}

		if ( longest > 0 )
		{
			Dentry * root = mnt_table[ longest_prefix ]->getSuperBlock()->getRoot();
			base = root;
			pathname = path_abs.substr( longest );
			if ( pathname.empty() )
				dirname.clear();
			else
				pathbuild();
			return root;
		}
		else return nullptr;
	}

	Dentry * Path::pathSearch( bool parent )
	{
		/// @todo 1: 通过路径名查找文件
		return nullptr;
	}

} // namespace fs

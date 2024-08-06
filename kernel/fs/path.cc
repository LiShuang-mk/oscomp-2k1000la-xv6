//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//
#include "pm/process_manager.hh"

#include "fs/path.hh"
#include "fs/file/file.hh"
#include "fs/file/normal.hh"
#include "fs/dentry.hh"
#include "fs/fs_defs.hh"
#include "fs/fat/fat32fs.hh"

#include "pm/process_manager.hh"
#include <EASTL/unordered_map.h>

namespace fs
{
	eastl::unordered_map<eastl::string, FileSystem *> mnt_table;


	/// 这里的base_文件需要有dentry，目前只有normalfile有dentry
	Path::Path( const eastl::string& path_, file *base_ )
		: pathname( path_ )
	{

		fs::normal_file *file = static_cast<fs::normal_file *>( base_ );
		if( file == nullptr ) 
			log_panic("Path: base file is not a normalfile");
		
		base = file->getDentry();
		pathbuild();
	}

	Path::Path( const char *str_, file *base_ )
		: pathname( str_ )
	{
		fs::normal_file *file = static_cast<fs::normal_file *>( base_ );
		if( file == nullptr ) 
			log_panic("Path: base file is not a normalfile");
		
		base = file->getDentry();
		pathbuild();
	}

	Path::Path( file *base_ )
	{
		fs::normal_file *file = static_cast<fs::normal_file *>( base_ );
		if( file == nullptr )  
			log_panic("Path: base file is not a normalfile");
		
		base = file->getDentry();
		pathbuild();
	}

	void Path::pathbuild()
	{
		//[[maybe_unused]]Dentry *root = mnt_table[ "/" ]->getRoot();
		if ( pathname.size() < 1 ) { base = pm::k_pm.get_cur_pcb()->get_cwd(); return; }	
		else if ( pathname[ 0 ] == '/' ) { base = mnt_table[ "/" ]->getRoot(); }
		else if ( base == nullptr ) { base = pm::k_pm.get_cur_pcb()->get_cwd(); }

		log_info("base name is %s ", base->rName());
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
			for ( size_t i = 0; i < ( size_t ) dirnum; ++i ) 
			{ 
				dirname[ i ] = pathname.substr( ind[ i ][ 0 ], ind[ i ][ 1 ] ); 
			}

			delete[] ind;
		}
		return;
	}

	eastl::string Path::AbsolutePath() const
	{
		eastl::vector<eastl::string> absname = dirname;
		for ( dentry * entry = base;
			!( entry->isRoot() && entry->getNode()->getSb()->getFileSystem()->isRootFS() );
			entry = entry->getParent() )
		{
			absname.emplace( absname.begin(), entry->rName() );
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

	dentry * Path::pathHitTable()
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
			dentry * root = mnt_table[ longest_prefix ]->getSuperBlock()->getRoot();
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

	dentry * Path::pathSearch( bool parent )
	{
		/// @todo 1: 通过路径名查找文件
		
		if( pathname == "/" ) return mnt_table[ "/" ]->getSuperBlock()->getRoot();
		dentry * entry, * next;
		if ( base == nullptr ) { return nullptr; }	// 无效路径
		// else if( ( entry = pathHitTable()) != nullptr) { }// 查询挂载表，找到对应根目录
		// else { entry = base; }	// 未找到对应根目录，使用当前目录
		
		entry = base;
		int dirsize = dirname.size();

		for(int i=0; i < dirsize; i++)
		{
			log_trace( "pathSearch: dirname[%d]: %s", i, dirname[i] );
			while(entry->isMntPoint()) 
				log_panic("pathSearch: entry is a mount point"); 
			/// @todo 这里随后检查 是否是目录，文件的结构不完善
			// if( !( entry->getNode()->rMode() & (fs::File_dir << fs::FileAttrs::File_dir_s ) ) )
			// 	return nullptr; // 不是目录
			if( parent && i == dirsize - 1 ) return entry; // 返回父目录
			if( dirname[i] == "." ) next = entry;
			else if( dirname[i] == ".." ) next = entry->getParent();
			else {
				if( auto it = entry->EntrySearch( dirname[i] ); it != nullptr ) next = it;
				else return nullptr;
			}
			entry = next;
		}
		return entry;
	}

	int Path::mount( Path &dev, eastl::string fstype , uint64 flags, uint64 data)
	{
		fs::dentry *mntEnt = pathSearch();
		fs::dentry *devEnt = dev.pathSearch();
		
		if( mntEnt->getNode( )->getFS()->mount( devEnt, mntEnt, fstype ) == 0)
		{
			return 0;
		}
		return -1;
	}

	int Path::umount( uint64 flags )
	{
		fs::dentry *mnt = pathSearch();
		if( mnt->getParent()->getNode()->getFS()->umount( mnt ) == 0 )
		{
			mnt_table.erase( pathname );
            return 0;
		}
		return -1;
	}

	int Path::open( FileAttrs attrs_ )
	{
		dentry *den = pathSearch();

		if( !den )
			return -1;
		FileAttrs attrs = den->getNode()->rMode();
		if( attrs.filetype != FileTypes::FT_DIRECT 
			&& attrs_.u_write
			&& attrs_.o_write
			&& attrs_.g_write )
			log_error(" try to open a directory with write mode");

		if( attrs_.filetype == FileTypes::FT_DIRECT 
			&& attrs.filetype != FileTypes::FT_DIRECT )
			log_error(" try to open a not directory file as directory ");
		
	    fs::normal_file *f = new fs::normal_file( attrs_, den );
		pm::Pcb *cur_proc = pm::k_pm.get_cur_pcb();
		int fd = pm::k_pm.alloc_fd( cur_proc, f );

		if( fd < 0 )
			return -1;
		return fd;
	}
} // namespace fs

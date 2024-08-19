//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

#include "fs/fs.hh"
#include "fs/fs_defs.hh"
#include "fs/file/file_defs.hh"
#include "fs/dentry.hh"
#include "fs/inode.hh"

namespace pm
{
	namespace ipc
	{
		class Pipe;
	}

}
namespace fs
{

	class Kstat
	{
	public:
		dev_t dev;
		uint64 ino;		
		mode_t mode;
		uint32 nlink;
		uint32 uid;     //user id
		uint32 gid;     //group id
		dev_t rdev;
		uint64 __pad;
		uint64 size;
		uint32 blksize;	
		int __pad2;	
		uint64 blocks;
		long st_atime_sec;
		long st_atime_nsec;
		long st_mtime_sec;
		long st_mtime_nsec;
		long st_ctime_sec;
		long st_ctime_nsec;

		Kstat() = default;
		Kstat( const Kstat& ks_ ) :
			dev( ks_.dev ), ino( ks_.ino ), mode( ks_.mode ),nlink( ks_.nlink ), 
			uid( ks_.uid ), gid( ks_.gid ), rdev( ks_.rdev ), __pad( ks_.__pad ),
			size( ks_.size ), blksize( ks_.blksize ),__pad2( ks_.__pad2 ), blocks( ks_.blocks ) 
		{
		};
		Kstat( dentry *dentry )   //@TODO: 通过De初始化kstat不完全
		{
			if( dentry )
			{
				Inode *node = dentry->getNode();
				if( node )
				{
					if( dentry->getNode()->rMode().filetype == fs::FileTypes::FT_DEVICE)
						dev = node->rDev();
					else 
						dev = -1;
					ino = node->rIno();
					mode = node->rMode().transMode(); // process mode 
					nlink = 1;
					uid = 0;
					gid = 0;
					rdev = 0;
					size = node->rFileSize();
					__pad = 0;
					blksize = node->getSb()->rBlockSize();
					blocks = node->getSb()->rBlockNum();	
					__pad2 = 0;
					st_atime_nsec = 0;
					st_atime_sec = 0;
					st_mtime_nsec = 0;
					st_mtime_sec = 0;
					st_ctime_nsec = 0;
					st_ctime_sec = 0;
				}
			}
		}
		
		Kstat(pm::ipc::Pipe *pipe) :
			dev( 0 ), ino( 1000000 ), mode( FT_PIPE ), nlink( 1 ), 
			uid( 0 ), gid( 0 ), rdev( 0 ), __pad( 0 ),
			size( 0 ), blksize( 0 ), __pad2( 0 ), blocks( 0 )
		{
		};

		Kstat( FileTypes f_type ) :
			dev( 0 ), ino( 1000000 ), mode( FT_NONE ), nlink( 1 ), 
			uid( 0 ), gid( 0 ), rdev( 0 ), __pad( 0 ),
			size( 0 ), blksize( 0 ), __pad2( 0 ), blocks( 0 )
		{
		};
		~Kstat() = default;
	};
} // namespace fs

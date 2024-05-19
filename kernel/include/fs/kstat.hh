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
#include "fs/file_defs.hh"
#include "fs/dentry.hh"
#include "fs/inode.hh"

namespace fs
{

	class Kstat
	{
	public:
		dev_t dev;
		uint64 ino;
		uint32 nlink;
		mode_t mode;
		uint32 uid;     //user id
		uint32 gid;     //group id
		dev_t rdev;
		uint64 __pad;
		uint64 size;
		size_t blksize;
		uint64 __pad2;
		uint64 blocks;

		Kstat() = default;
		Kstat( const Kstat& ks_ ) :
			dev( ks_.dev ), ino( ks_.ino ), nlink( ks_.nlink ), mode( ks_.mode ),
			uid( ks_.uid ), gid( ks_.gid ), rdev( ks_.rdev ), __pad( ks_.__pad ),
			size( ks_.size ), blksize( ks_.blksize ), __pad2( ks_.__pad2 ), blocks( ks_.blocks )
		{
		};
		Kstat( Dentry *dentry );
		
  //Kstat(Pipe *pipe){};
		Kstat( FileTypes f_type ) :
			dev( 0 ), ino( -1 ), nlink( 1 ), mode( FT_NONE ),
			uid( 0 ), gid( 0 ), rdev( 0 ), __pad( 0 ),
			size( 0 ), blksize( 0 ), __pad2( 0 ), blocks( 0 )
		{
		};
		~Kstat() = default;
	};
} // namespace fs

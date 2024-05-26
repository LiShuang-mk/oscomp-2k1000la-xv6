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
#include <EASTL/unordered_map.h>

namespace fs
{
	class Inode;

	class Dentry
	{               // Directory Entry， 目录项，存在于缓存，不用交由下层FS实现
		public:
			Dentry() = default;
			Dentry( const Dentry& dentry ) = delete;
			Dentry & operator=( const Dentry& dentry ) = delete;
			//Dentry( Dentry *parent_, Inode *node_, eastl::string name_ ) : parent( parent_ ), node( node_ ), name( name_ ), isMountPoint( false ) {};
			virtual Dentry * EntrySearch( eastl::string name ) = 0;
			virtual Dentry * EntryCreate( eastl::string name, uint32 mode ) = 0 ;
			virtual Inode * getNode() = 0;
			virtual bool isRoot() = 0;
			virtual Dentry *getParent() = 0;
			virtual eastl::string getName() = 0; 
	};

} // namespace fs

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
		Dentry *parent;
		Inode *node;
		eastl::string name;
		bool isMountPoint;
		eastl::unordered_map<eastl::string, Dentry *> children;
		//Dentry *children[]; //the num of size is just for test
		//unordered_map<string, Dentry *> children;
	public:
		Dentry() = delete;
		Dentry( const Dentry& dentry ) = delete;
		Dentry & operator=( const Dentry& dentry ) = delete;
		Dentry( Dentry *parent_, Inode *node_, eastl::string name_ ) : parent( parent_ ), node( node_ ), name( name_ ), isMountPoint( false ) {};
		Dentry * EntrySearch( Dentry * self, eastl::string name );
		Dentry * EntryCreate( Dentry * self, eastl::string name, uint32 mode );
		inline void setMountPoint() { isMountPoint = true; }
		inline void cleanMountPoint() { isMountPoint = false; }
		inline bool isMount() const { return isMountPoint; }
		//inline int readDir(DStat *buf, uint32 len, uint64 off_) {return node->readDir(buf, len, off_);}
		inline eastl::string getName() const { return name; }
		inline Dentry * getParent() const { return parent; }
		inline Inode * getNode() const { return node; }
		inline bool isRoot() const { return parent == nullptr; }
	};

} // namespace fs

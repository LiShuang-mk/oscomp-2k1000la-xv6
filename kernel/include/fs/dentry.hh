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
#include <EASTL/vector.h>

using eastl::vector;
namespace fs
{
	class Inode;
	namespace dentrycache
	{
		class dentryCache;
	}

	enum DentryType
    {
            RAMFS_DENTRY = 0,
            FAT32_DENTRY = 1,
    };

	namespace fat
	{
		class Fat32SuperBlock;
		class Fat32FS;	
	}

	namespace ramfs
	{
		class RamFS;
		class RamInode;
		class RamFSSb;
	}

	class dentry
	{
		friend class dentrycache::dentryCache;
		private:
			eastl::string name;
			eastl::unordered_map<eastl::string, dentry *> children;
			Inode *node;

			dentry *parent;
			uint Did; // dentry id
			bool isroot;

		public:
			dentry() = default; //{ name.clear(); children.clear(); node = nullptr;  parent = nullptr; isroot = false; };
			dentry( const dentry& ) = default;
			dentry & operator=( const dentry& ) = default;
			dentry( eastl::string name, Inode* node, dentry* parent ) : name( name ), node( node ), parent( parent ) {}
			dentry( uint did ) : Did( did ) {}
			~dentry() = default;

			dentry *EntrySearch( eastl::string name );
			dentry *EntryCreate( eastl::string name, uint32 mode );
			Inode *getNode();	
			bool isRoot();
			dentry *getParent() { return parent == nullptr ? nullptr : parent ;};
			eastl::string rName() { return name; };
			uint getDid() { return Did; };
			void reset( vector<int> &bitmap );
			eastl::unordered_map<eastl::string, dentry*> &getChildren() { return children; };
			//bool is_root();
			bool isMntPoint();
			void delete_child( eastl::string name ) { children.erase( name ); };
			void setParent( dentry *parent ) { this->parent = parent; };
		
		public:
			void init( uint32 dev, ramfs::RamFS *fs );	
			void init( uint32 dev, fat::Fat32SuperBlock *sb, fat::Fat32FS* fs, eastl::string rootname );
			void init( eastl::string name_, Inode *node_, dentry *parent_ ) ;
			void rootInit( Inode *node, eastl::string rootname );

			void printChildrenInfo();

	};
} // namespace fs

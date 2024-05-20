//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dentry.hh"
#include "fs/inode.hh"

namespace fs
{
	Dentry* Dentry::EntrySearch( Dentry *self, eastl::string name )
	{
		if ( children.find( name ) != children.end() )
			return children[ name ];

		if ( auto subnode = node->lookup( name ) )
		{
			auto subDEntry = new Dentry( self, subnode, name );
			children.insert( { name, subDEntry } );
			return subDEntry;
		}
		return nullptr;
	}

	Dentry* Dentry::EntryCreate( Dentry *self, eastl::string name, mode_t mode )
	{
		if ( children.find( name ) != children.end() )
		{
			log_error( "File %s already exists!\n", name.c_str() );
			return nullptr;
		}

		if ( auto subnode = node->mknode( name, mode ) )
		{
			auto subDEntry = new Dentry( self, subnode, name );
			children.insert( { name, subDEntry } );
			return subDEntry;
		}
		return nullptr;
	}
} // namespace fs

#include "fs/ramfs/ramfsDentry.hh"
#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfsSb.hh"
#include "fs/ramfs/ramfs.hh"

#include "fs/inode.hh"
#include "fs/fs.hh"

#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace fs{
    
    namespace ramfs{
        
        bool S_ISDIR( uint32 mode ){
            /// @todo check file is directory or not
            /// @todo complement this function, it is not complete,
            //return ( mode & 0xF000 ) == 0x4000;
            return mode > 0;
        }

        Dentry *RamFSDen::EntrySearch( eastl::string name ){
            auto it = children.find( name );
            if( it != children.end() ){
                return it->second;
            }
            return nullptr;
        }

        Dentry *RamFSDen::EntryCreate( eastl::string name, uint32 mode ){
            RamFS *nodefs;
            if( ( nodefs = dynamic_cast<RamFS *>( node->getFS() ) ) == nullptr ){
                log_error( "RamFSDen::EntryCreate: nodefs is not RamFS" );
                return nullptr;
            }
            RamInode *inode = new RamInode( nodefs, node->rIno(), S_ISDIR( mode ) );
            RamFSDen *dentry = new RamFSDen( name, inode );
            children[ name ] = dentry;
            return dentry;
        }

        Inode *RamFSDen::getNode(){
            return node;
        }

    }
}
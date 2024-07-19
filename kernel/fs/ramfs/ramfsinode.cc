#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"

namespace fs{
    
    namespace ramfs{
      

        SuperBlock *RamInode::getSb() const{
            return belong_fs->getSuperBlock();
        }

        FileSystem *RamInode::getFS() const{
            return belong_fs;
        }

        Inode *RamInode::mknode(eastl::string name, mode_t mode) 
        {
            return belong_fs->getSuperBlock()->allocInode( mode );
        }
    }
}
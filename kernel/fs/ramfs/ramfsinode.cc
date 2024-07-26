#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"

#include "hsai_global.hh"
#include "device_manager.hh"

namespace fs{
    
    namespace ramfs{
      

        SuperBlock *RamInode::getSb() const{
            return belong_fs->getSuperBlock();
        }

        FileSystem *RamInode::getFS() const{
            return belong_fs;
        }

        Inode *RamInode::mknode(eastl::string name, FileAttrs attrs, eastl::string dev_name ) 
        {
            return belong_fs->getSuperBlock()->allocInode( attrs, dev_name );
        }

        dev_t RamInode::rDev() const
        {
            if( dev_name.empty() )
                log_error("This is not a valid device file");
            return hsai::k_devm.search_device( dev_name.c_str() );
        }
    }
}
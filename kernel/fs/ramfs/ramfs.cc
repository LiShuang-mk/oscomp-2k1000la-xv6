#include "fs/ramfs/ramfsSb.hh"
#include "fs/ramfs/ramfs.hh"
#include "fs/dentry.hh"

namespace fs{

    namespace ramfs{

        void RamFS::initfd(){
            /// @todo main purpose is init /device, init device and set it as root
            
            return;
        }

        RamFS::RamFS(){
            sb = new RamFSSb( this );
        }
        
    }
}
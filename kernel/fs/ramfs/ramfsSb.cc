#include "fs/ramfs/ramfsSb.hh"
#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"

namespace fs
{
    namespace ramfs
    {
        Inode *RamFSSb::allocInode( FileAttrs attrs , eastl::string dev_name)
        {
            return new RamInode( static_cast< RamFS *>(fs), ++ino, attrs, dev_name );
        }
    }
}// namespace fs
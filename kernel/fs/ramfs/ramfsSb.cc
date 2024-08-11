#include "fs/ramfs/ramfsSb.hh"
#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"

namespace fs
{
    namespace ramfs
    {
        Inode *RamFSSb::allocInode( FileAttrs attrs , eastl::string dev_name)
        {
            RamFS *ram_fs = static_cast<RamFS *>(fs);
            return new RamInode( ram_fs, ram_fs->alloc_ino(), attrs, dev_name );
        }
    }
}// namespace fs
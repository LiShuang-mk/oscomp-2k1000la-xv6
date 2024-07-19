#include "fs/ramfs/ramfsSb.hh"
#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"

namespace fs
{
    namespace ramfs
    {
        Inode *RamFSSb::allocInode( mode_t mode )
        {
            return new RamInode( static_cast<RamFS *>(fs), ++ino, mode, ISDIR(mode) );
        }
    }
}// namespace fs
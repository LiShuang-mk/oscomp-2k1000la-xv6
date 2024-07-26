#include "fs/fat/fat32Sb.hh"
#include "fs/fat/fat32inode.hh"
#include "fs/fat/fat32fs.hh"
#include "fs/dentry.hh"

namespace fs
{
    namespace fat
    {
        Inode * Fat32SuperBlock::allocInode( FileAttrs attrs, eastl::string dev_name )
        {
            return new Fat32Inode( static_cast<Fat32FS *>(_fs), attrs, 
                                                    ++ino, dev_name );
        }
    }
}//namespace fs
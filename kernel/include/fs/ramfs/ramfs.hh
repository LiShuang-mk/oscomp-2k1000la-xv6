#include "types.hh"
#include "klib/common.hh"
#include "fs/fs.hh"

#include <EASTL/string.h>

namespace fs{
    
    class Dentry;

    namespace ramfs{

        class RamFSSb;

        class RamFS : public FileSystem{


            uint _device;
            eastl::string _fstype;
            RamFSSb *sb;    // super block            
            bool _isroot;

            public:
                void initfd(); // init ramfs, especially for some essential directories

                RamFS();
                RamFS( const RamFS &fs ) = default;
                RamFS( bool isroot_ ) : _isroot( isroot_ ) {};
                
                ~RamFS() = default;

                void init( int dev, uint64 start_lba, bool is_root = false );
                bool isRootFS() const override { return false; };
                SuperBlock *getSuperBlock() const override { return sb; };
                size_t rBlockSize() const override { return 512; }; // RamFS does not have block size
                long rBlockNum() const override { return 0; }; // RamFS does not have block number
                long rBlockFree() const override { return 0; }; // RamFS does not have free block number
                long rMaxFile() const override { return 0; }; // RamFS does not have max file number
                long rFreeFile() const override { return 0x100; }; // RamFS does not have free file number
                size_t rNamelen() const override { return 512; }; // RamFS does not have max name length
                Dentry *getRoot() const override { return nullptr; }; // RamFS does not have root dentry
                Dentry *getMntPoint() const override { return nullptr; }; // RamFS does not have mount point dentry
        };
    } // ramfs
}
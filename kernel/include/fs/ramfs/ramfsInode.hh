#include "types.hh"
#include "fs/inode.hh"
#include "fs/kstat.hh"

#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace fs{
    
    class Kstat;
    class FileSystem;
    namespace ramfs{
        
        class RamFSSb;
        class RamFS;

        class RamInode : public Inode{

            friend RamFS;
            private:
                RamFS *belong_fs;
                const uint ino;
                bool is_dir;
                eastl::vector<uint8> data;
                mode_t mode;
                int dev;
            public:
                RamInode() = default;
                RamInode( RamFS *fs_, uint ino_, mode_t mode_, bool isDir = false, int dev_ = -1 ) : belong_fs( fs_ ), ino( ino_ ), is_dir( isDir ), mode(mode_), dev(dev_) {}
                RamInode( RamFS *fs_, uint ino_, bool isDir) : belong_fs( fs_ ), ino( ino_ ), is_dir( isDir ), mode( 0 ) {}
                ~RamInode() = default;
                
                Inode *lookup( eastl::string dirname ) override { return nullptr ; } ;
                Inode *mknode( eastl::string dirname, mode_t mode, int dev = -1 ) override  ;
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override { return 0 ; } ;
                size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override { return  0; } ;

                mode_t rMode() const override { return mode; };
                dev_t rDev() const override { return dev; } ;
                uint64 rFileSize() const override { return data.size(); };
                uint64 rIno() const override { return ino; };
                SuperBlock *getSb() const override;
                FileSystem *getFS() const override;
        };
    }
}

#include "types.hh"
#include "fs/inode.hh"
#include "fs/kstat.hh"

#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace fs{
    
    class Kstat;

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

            public:
                RamInode() = default;
                RamInode( RamFS *fs_, uint ino_, bool is_dir_ = false ) : belong_fs( fs_ ), ino( ino_ ), is_dir( is_dir_ ) {}
                ~RamInode() = default;
                
                Inode *lookup( eastl::string dirname, off_t off_ = 0 ) override { return nullptr ; } ;
                Inode *mknode( eastl::string dirname, uint32 mode ) override { return nullptr ; } ;
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override { return 0 ; } ;
                size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override { return  0; } ;

                mode_t rMode() const override { return 0577; };
                dev_t rDev() const override { return 0; } ;
                uint64 rFileSize() const override { return data.size(); };
                uint64 rIno() const override { return ino; };
                SuperBlock *getSb() const override;
                FileSystem *getFS() const override;
        };
    }
}

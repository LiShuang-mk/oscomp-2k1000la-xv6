#include "types.hh"
#include "fs/fs.hh"
#include "fs/fs_defs.hh"



namespace fs{
    
    class Inode;
    class FileSystem;
    class Dentry;
    
    namespace ramfs{

        class RamFSSb : public fs::SuperBlock {
            FileSystem *fs;
            bool is_valid;

            public:
                RamFSSb() = default;
                RamFSSb(FileSystem * _fs) : fs(_fs), is_valid(true) {}
                ~RamFSSb() = default;
			    Dentry *getRoot() const override { return fs->getRoot(); };
                Dentry *getMntPoint() const override { log_panic("RamFS getMntPoint is forbidden!"); return nullptr; }
                FileSystem *getFileSystem() const override { return fs; }
                bool isValid() const override { return true; }
                uint32 rDefaultMod() const override { return 0644; } 
                size_t rBlockSize() const override { return 0; }
                size_t rBlockNum() const override { return 0; }
        };
    }
}
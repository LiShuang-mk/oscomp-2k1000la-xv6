#pragma once

#include "types.hh"
#include "fs/fs.hh"

namespace fs{
    
    class Inode;
    class FileSystem;
    class Dentry;
    
    namespace ramfs{


        class RamFSSb : public fs::SuperBlock {
            FileSystem *fs;
            bool is_valid;
            uint32 ino;
            public:
                RamFSSb() { fs = nullptr; is_valid = false; ino = 0; };
                RamFSSb(FileSystem * _fs) : fs(_fs), is_valid(true) {}
                ~RamFSSb() = default;
			    dentry *getRoot() const override { return fs->getRoot(); };
                dentry *getMntPoint() const override { log_panic("RamFS getMntPoint is forbidden!"); return nullptr; }
                FileSystem *getFileSystem() const override { return fs; }
                bool isValid() const override { return true; }
                uint32 rDefaultMod() const override { return fs->rDefaultMod(); } 
                size_t rBlockSize() const override { return 0; }
                size_t rBlockNum() const override { return 0; }
                Inode*  allocInode( mode_t mode, int dev = -1 )  override;
        };
    }
}
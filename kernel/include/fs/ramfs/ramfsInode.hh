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

#pragma pack(8)
        class RamInode : public Inode{

            friend RamFS;
            protected:
                RamFS *belong_fs;
                const uint ino;
                eastl::vector<uint8> data;
                bool is_dir;
                FileAttrs attrs; // filetype
                eastl::string  dev_name;  // 只有设备文件这里不是空
            public:
                bool readable = false;
                RamInode() = default;
                    RamInode(   RamFS *fs_, uint ino_, 
                                FileAttrs attr, eastl::string devname = "" )
                                : belong_fs( fs_ )
								, ino( ino_ )
								, is_dir( attr.filetype == FileTypes::FT_DIRECT )
								, attrs( attr )
								, dev_name(devname) {}
                    RamInode( RamFS *fs_, uint ino_, bool isDir) 
                                : belong_fs( fs_ ), ino( ino_ ), is_dir( isDir ) {}
                    ~RamInode() = default;
                    
                    Inode *lookup( eastl::string dirname ) override { return nullptr ; } ;
                    Inode *mknode( eastl::string dirname, FileAttrs attrs, eastl::string dev_name = "" ) override;
                    size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override;
                    size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override { return  0; } ;

                    FileAttrs rMode() const override { return attrs; };
                    dev_t rDev() const override ;
                    uint64 rFileSize() const override { return data.size(); };
                    uint64 rIno() const override { return ino; };
                    SuperBlock *getSb() const override;
                    FileSystem *getFS() const override;
                    int readlinkat( char *buf, size_t len ) override { return 0; };

					virtual size_t readSubDir( ubuf &dst, size_t off) override{
						return 0;
					}
		};
		// int ri = sizeof(RamInode );
#pragma pack()


        class Exe : public RamInode
        {
            public:
                Exe( RamFS *fs, uint ino ) : ramfs::RamInode(fs, ino, false) {};
                int readlinkat( char *buf, size_t len ) override;
        };

        class Mount : public RamInode
        {
            public:
                Mount( RamFS *fs, uint ino, FileAttrs attrs ) : ramfs::RamInode( fs, ino, attrs ) {};
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override ;
        };

        class SymbleLink : public RamInode
        {   
            private:
                eastl::string target_path;  //
            public:
                SymbleLink( RamFS *fs, uint ino, FileAttrs attrs, eastl::string target ) : ramfs::RamInode( fs, ino, attrs ), target_path( target ) {};
                size_t nodeRead( uint64 dst, size_t off_, size_t len_ ) override;
                eastl::string rTargetPath() { return target_path; }; 
        };

        class MemInfo : public RamInode
        {
            public:
                MemInfo( RamFS *fs, uint ino ) : ramfs::RamInode( fs, ino, false ) {};
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override;
        };

        class RTC : public RamInode
        {   
            private:
                dev_t dev_;
            public:
                RTC( RamFS *fs, uint ino, FileAttrs attrs, dev_t dev ) : ramfs::RamInode( fs, ino, attrs ) , dev_(dev) {};
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override;
                dev_t rDev() const override { return dev_; };
        };

        class Device : public RamInode
        {
            private:
                dev_t dev_;
            public:
                Device( RamFS *fs, uint ino, FileAttrs attrs, dev_t dev ) : ramfs::RamInode( fs, ino, attrs ) , dev_(dev) {};
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override;
                size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override;
                dev_t rDev() const override { return dev_; };
        };

        class Zero : public RamInode
        {
            private:
                dev_t dev_;
            public:
                Zero( RamFS *fs, uint ino, FileAttrs attrs, dev_t dev ) : ramfs::RamInode( fs, ino, attrs ), dev_( dev ) {};
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override;
                size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override { return len_; };
                dev_t rDev() const override { return dev_; };
        };

        class Null : public RamInode
        {
            private:
                dev_t dev_;
            public:
                Null( RamFS *fs, uint ino, FileAttrs attrs, dev_t dev ) : ramfs::RamInode( fs, ino, attrs ), dev_( dev ) {};
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override { return -1; };
                size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override { return len_; };
        };

        class Normal : public RamInode
        {
            eastl::string data;
            public:
                Normal( RamFS *fs, uint ino, FileAttrs attrs, eastl::string data_ ) : ramfs::RamInode( fs, ino, attrs ), data( data_ ) {};
                Normal( RamFS *fs, uint ino, FileAttrs attrs ) : ramfs::RamInode( fs, ino, attrs ) { data.clear(); };
                size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override;
                size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override;
                size_t rFileSize() const override { return data.length(); };
        };
    }
}

#include "../types.hh"
#include "../smp/lock.hh"

#define NDIRECT 12

namespace fs
{
        class FileSystem;
        class Dentry;
        class Inode;
        class File;
        class DStat;

        class SuperBlock{  
           SuperBlock() = default;
           SuperBlock(const SuperBlock& sb) = delete;
           virtual ~SuperBlock() = default;
           SuperBlock & operator=(const SuperBlock& sb);
           virtual *Dentry getRoot() const = 0;     // get root dentry  
           virtual *Dentry getMntPoint() const = 0; // get mount point dentry
           virtual *FileSystem getFileSystem() const = 0; // get file system
           virtual bool isValid() const {return true ;} // check if super block is valid
           virtual mode_t rDefaultMod() const = 0; // get default mode
           virtual inode *allocInode() = 0; // allocate inode
           virtual void destroyInode(inode *inode) = 0; // destroy inode
        };
        
        class FileSystem{
            FileSystem() = default;
            FileSystem(CONST FileSystem& fs) = delete;
            virtual ~FileSystem() = default;
            FileSystem& operator=(const FileSystem& fs);
            virtual char *rFSTyoe() const = 0; // get file system type
            virtual char *rKey() const = 0; // get fs's key in mount map
            virtual bool isRootFS() const = 0; // check if it is root fs
            virtual SuperBlock *getSuperBlock() const = 0; // get super block
            virtual int ldSuperBlock(uint64 dev, Dentry * mnt) = 0; // load super block
            virtual void unInstall() = 0; // uninstall file system
            virtual long rMagic() const = 0; // get magic number
            virtual long rBlockSize() const = 0; // get block size
            virtual long rBlockNum() const = 0; // get block number
            virtual long rBlockFree() const = 0; // get free block number
            virtual long rMaxFile() const = 0; // max file num when read
            virtual long rFreeFile() const = 0; // max file num when read free block
            virtual long rNameLen() const = 0; // get max name length
        };

        Inode * InodeRef;
        class Inode{
            public:
                Inode() = default;
                Inode(const Inode& inode) = delete;
                virtual ~Inode() = default;
                Inode& operator=(const Inode& inode);
                
                //virtual void link(const char *name,InodeRef nod) = 0;
                virtual int nodeHardUnlink() = 0;
                virtual InodeRef lookup(const char *dirname, uint32 off_ = nullptr) = 0;;
                virtual InodeRef mknode(const char *name, mode_t mode) = 0;
                virtual int entSymLink(const char *target) = 0;

                virtual void nodeRemove() = 0;
                virtual int chMod(mode_t mode) = 0;
                //virtual int chOwn(uid_t uid, gid_t gid) = 0; //change group owner
                virtual void nodeTrunc() = 0;
                virtual int nodeRead(uint64 dst_, uint32 off_, uint32 len_) = 0;
                virtual int nodWrite(uint64 src_, uint32 off_, uint32 len_) = 0;
                
                //void readvec();
                //void readPages();

                virtual int readLink(char *buf, uint64 len) = 0;
                virtual int readDir(DStat *buf, uint32 len, uint64 off_) = 0;
                virtual int ioctl(uint64 req, uint64 arg) = 0;
                virtual mode_t rMode() const = 0; // get mode
                virtual dev_t rDev() const = 0; // get device number
                virtual off_t rFIleSize()   const = 0; // get file size
                virtual ino_t rIno() const = 0; // get inode number
                // Ctime, Mtime, Atime temporarily not implemented
                virtual bool Empty() const = 0; // check if inode is empty
                virtual SuperBlock getSb() const = 0; // get super block
                virtual int create(inode *dir, dentry *dentry, char *name, mode_t mode) = 0;
                virtual int mkdir(inode *dir, dentry *dentry, char *name, mode_t mode) = 0;
        };  
        
        Dentry * DentryRef;
        class Dentry{               // Directory Entry， 目录项，存在于缓存，不用交由下层FS实现
                Dentry *parent;
                Inode *node;
                char *name;
                bool isMountPoint;
                //unordered_map<string, Dentry *> children;
            public:
                Dentry() = delete;
                Dentry(const Dentry& dentry) = delete;
                Dentry & operator=(const Dentry& dentry) = delete;
                Dentry(Dentry *parent_, Inode *node_, const char *name_, bool isMountPoint_) : parent(parent_), node(node_), name(name_), isMountPoint(isMountPoint_) {};
                DentryRef EntrySearch(DentryRef self, const char *name);
                DentryRef EntryCreate(DentryRef self, const char *name, mode_t mode);
                inline void setMountPoint() {isMountPoint = true;}
                inline void cleanMountPoint() {isMountPoint = false;}
                inline bool isMount() const {return isMountPoint;}
                //inline int readDir(DStat *buf, uint32 len, uint64 off_) {return node->readDir(buf, len, off_);}
                inline char *getName() const {return name;}
                inline DentryRef getParent() const {return parent;}
                inline InodeRef getNode() const {return node;}
                inline bool isRoot() const {return parent == nullptr;}
        };

        enum FileTypes{
            FT_NONE,
            FT_PIPE,
            FT_STDIN,
            FT_STDOUT,
            FT_ERROR,
            FT_INODE,
            FT_DEVICE,
            FT_ENTRY
        };

        enum class FileOp:uint16 {none = 0x00, read = 0x01, write = 0x02, append = 0x04,};
        
        enum FileAuthority{
            FA_READ = 0x01,
            FA_WRITE = 0x02,
            FA_APPEND = 0x04,
        };
        
        union FileOps {
            struct {
                int r:1;    //read
                int w:1;    //write
                int a:1;    //append
            } fields;   //file's  authority
            FileOp rwa;
            FIleOps(FileOp op_ = FileOp::none) : rwa(op_) {}
            FIleOps(int flags_){
                fields.r = flags_ & FA_READ;
                fields.w = flags_ & FA_WRITE;
                fields.a = flags_ & FA_APPEND;
            }
        };

        class Kstat{
            public:
                dev_t dev;
                uint64 ino;
                uint32 nlink;
                mode_t mode;
                uint32 uid;     //user id
                uint32 gid;     //group id
                uint64 rdev;
                uint64 __pad;
                off_t size;
                uint32 blksize;
                int __pad2;
                uint64 blocks;

                Kstat() = default;
                Kstat(const Kstat& ks_) : 
                    dev(ks_.dev), ino(ks_.ino), nlink(ks_.nlink), mode(ks_.mode), 
                    uid(ks_.uid), gid(ks_.gid), rdev(ks_.rdev), __pad(ks_.__pad), 
                    size(ks_.size), blksize(ks_.blksize), __pad2(ks_.__pad2), blocks(ks_.blocks) {};
                Kstat(Dentry *dentry) : 
                    dev(dentry->getNode()->rDev()), 
                    ino(dentry->getNode()->rIno()), 
                    nlink(1), 
                    mode(dentry->getNode()->rMode()), 
                    uid(0), gid(0), rdev(0), __pad(0), 
                    size(dentry->getNode()->rFIleSize()), 
                    __pad2(0), blocks(size / blksize),
                    blksize(dentry->getNode->getSb()->getFileSystem()->rBlockSize()) {};
                Kstat(Pipe *pipe){};
                Kstat(FileTypes f_type) : 
                    dev(0),ino(-1),mode(FT_NONE),uid(0),
                    gid(0),rdev(0),__pad(0),size(0),
                    blksize(0),__pad2(0),blocks(0),nlink(1) {};
                ~Kstat() = default;
        };


        class File{
                int flags;
                FileTypes type;
                FileOps *ops;
                union Data{
                    private:
                        struct {const FileTypes type_; Kstat kst_} dv_;
                        //struct {const FileTypes type_; Pipe *pipe_} pp_;
                        struct {const FileTypes type_; Kstat kst_, Dentry *dentry_, off_t off_} en_;
                        inline bool ensureDev() const { return dv_.type == FT_NONE || dv_.type_ == FT_STDIN || dv_.type_ == FT_STDOUT || dv_.type_ == FT_DEVICE ;};
                        inline bool ensureEntry() const { return en_.type == FT_ENTRY;};
                    public:
                        Data(FileTypes type) : dv_{type, type}, { ensureDev(); }
                        Data(Dentry *de_) : en_({FT_ENTRY,de_, de_, 0}) { ensureEntry(); }
                        ~Data() = default;
                        inline Dentry* get_Entry() const {ensureEntry(); return en_.dentry_; }
                        inline Kstat & get_Kstat() { if(ensureEntry()) return en_.kst_; return nullptr;}
                        inline off_t & get_off() { if(ensureEntry()) return en_.off_; return -1;}
                        inline FileTypes get_Type() const {return en_.type_;}
                    }data;
                File(FileTypes type_) : data(type_) {}
                File(FileTypes type_, FileOps ops_ = FileOp::none) : data(type_), ops(ops_) {}
                File(FileTypes type_, int flags_) : data(type_), ops(flags_) {}
                File(Dentry *de_, int flags_) : data(de_), ops(flags_), flags(flags_) {}
                ~File() = default;

                int write(const void *buf, size_t len);
                int read(void *buf, size_t off_ = -1, bool update);

       };

       class Path {
            private:
                Dentry *base;
                char *path;

            public:
                Path() = default;
                Path(const Path& path) = default;
                Path(const char *path_, File *base) : path(path_),
       };
}

#include "fs/ramfs/ramfs.hh"
#include "fs/dentrycache.hh"
#include "fs/dentry.hh"
#include "fs/file.hh"

#include "fs/fat/fat32fs.hh"
#include "EASTL/queue.h"

using namespace fs::dentrycache;

namespace fs{

    namespace ramfs{

        RamFS k_ramfs;
        
        Dentry *RamFS::getRoot() const
        {
            return _root;
        }

        void RamFS::initfd(){
            /// @todo main purpose is init /device, init device and set it as root
           
            /// set device num
            _device = 0;
            /// set file system type
            _fstype = "ramfs";  
            /// super block has been initialized in constructor
            _root = new RamFSDen();
            _root->init( _device, this );
            _super_block = new RamFSSb( this );
            _mnt = _root;
            /// ramfs is temporary fs 
            _isroot = true;


            _root->EntryCreate( "dev", FileAttrs::File_dir << FileAttrs::File_dir_s ); // 暂时把第二个参数为正数认为是目录， 还要创建 /dev/sda1, 用来初始化ext4
            _root->EntryCreate( "proc", FileAttrs::File_dir << FileAttrs::File_dir_s );
            _root->EntryCreate( "sys", FileAttrs::File_dir << FileAttrs::File_dir_s );
            _root->EntryCreate( "tmp", FileAttrs::File_dir << FileAttrs::File_dir_s );
            _root->EntryCreate( "mnt", FileAttrs::File_dir << FileAttrs::File_dir_s );
            
            // init fat
            Dentry* dev = _root->EntrySearch( "dev" );
            if( !dev )
            {
                log_error("RamFS::initfd: mnt is nullptr");
                return;
            }
            dev->EntryCreate("vda2", FileAttrs::File_dir << FileAttrs::File_dir_s );
            _root->printChildrenInfo( );

            return;
        }

        RamFS::RamFS(){
            _super_block = new RamFSSb( this );
        }
        
        void RamFS::dentryCacheTest( )
        {
            Dentry *dentry = fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY );
			_root = static_cast<RamFSDen *>(dentry);
			//_root->init( _device, _super_block, this );
			_root->init( "/", nullptr, nullptr);
            k_dentryCache.touchDentry( _root );

            RamFSDen *dentry1;
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "dev", nullptr, _root );
            k_dentryCache.touchDentry( dentry1 );
            _root->add_children( dentry1 );
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "proc", nullptr, _root );
            k_dentryCache.touchDentry( dentry1 );
            _root->add_children( dentry1 );  

            // fat::Fat32Dentry *dentry2;
            // dentry2 = static_cast<fat::Fat32Dentry *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::FAT32_DENTRY ));
            // dentry2->init( dentry1, nullptr, "fatDentry" );

            RamFSDen *dev = static_cast<RamFSDen *>(_root->EntrySearch( "dev" ));// 准备在dev下创建sda1
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "sda1", nullptr, dev );
            k_dentryCache.touchDentry( dentry1 );
            dev->add_children( dentry1 );
            _root->printChildrenInfo( );

            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "sda2", nullptr, dev );
            k_dentryCache.touchDentry( dentry1 );
            dev->add_children( dentry1 );

            _root->printChildrenInfo();
            
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "sda3", nullptr, dev );
            k_dentryCache.touchDentry( dentry1 );
            dev->add_children( dentry1 );
            _root->printChildrenInfo();

            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "mnt", nullptr, _root );
            k_dentryCache.touchDentry( dentry1 );
            _root->add_children( dentry1 );
            _root->printChildrenInfo(); // add /mnt

            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            RamFSDen *mnt = static_cast<RamFSDen *> ( _root->EntrySearch( "mnt" ) ); 
            dentry1->init( "fat", nullptr, mnt );
            mnt->add_children( dentry1 );
            _root->printChildrenInfo(); // add /mnt/fat

			return;
        }


        // 关于mount， 目前只考虑挂载FS的情况，提供参数有  special, mnt, fstype, flags, data，
        // 首先对给定的 special 和 mnt的路径进行解析，拿到对应的dentry， 然后进行挂载，具体为
        // 对special 对应设备中的FS初始化，然后把他的 mnt指向 当前的挂载点供 卸载文件系统的时候恢复原dentry
        // 然后替换dentry 即可。 目前的flags 和 data先不进行考虑

        int RamFS::mount( Dentry *dev, Dentry *&mount, eastl::string fstype)
        {
            if( mount == nullptr || dev == nullptr )
            {
                log_error("RamFS::mount: mount or dev is nullptr");
                return -1;
            }

            if( mount->isRoot() || mount->isMntPoint() )
            {
                log_error("RamFS::mount: mount is root or mount point");
                return -1;
            }

            if( !ISDIR( mount->getNode()->rMode() ) )
            {
                log_error("RamFS::mount: mount is not a directory");
                return -1;
            }
            
            // 具体fs挂载初始化，先写个Fat32
            if( fstype == "fat32" || fstype == "vfat" )
            {
                // fs'_mnt 保存原挂载点信息
                fs::fat::Fat32FS *fatfs = new fs::fat::Fat32FS( false, mount );
                //fatfs->init( dev->getNode()->rDev(), 0, fstype );
                fatfs->init( 1, 0, fstype, mount->rName() );
                //Dentry *parent = mount->getParent();
                //log_info( "parent is %s", parent->getName().c_str() );
                //eastl::unordered_map<eastl::string, Dentry *> dev_children = fatfs->getRoot()->getChildren();
                // for( auto &p : dev_children )
                // {
                //     printf( "%s ", p.second->getName().c_str());
                // }
                //printf("\n%s ", mount->rName());
                mount->getParent()->getChildren()[mount->rName()] = fatfs->getRoot();
                for( auto it : mount->getChildren() )
                {
                    log_info("RamFS::mount: %s", it.second->getName().c_str());
                }
            }
            else
            {
                log_error("RamFS::mount: unknown file system type");
                return -1;
            }
            _root->printChildrenInfo();
            return 0;
        }

        void RamFS::traversal()
        {
            eastl::queue<fs::Dentry *> currentlevel;
            eastl::queue<fs::Dentry *> nextlevel;

            currentlevel.push( _root );

            while(!currentlevel.empty() )
            {
                fs::Dentry *dentry = currentlevel.front();
                currentlevel.pop();
                log_info("RamFS::traversal: %s", dentry->getName().c_str());
                eastl::unordered_map<eastl::string, fs::Dentry *> _children = dentry->getChildren();
                
                for( auto &child : _children )
                {
                    nextlevel.push( child.second );
                }
                
                if( currentlevel.empty() )
                {
                    currentlevel.swap( nextlevel );
                }
            }
            
        }
    }
}
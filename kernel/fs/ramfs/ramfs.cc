#include "fs/ramfs/ramfs.hh"
//#include "fs/ramfs/ramfsDentry.hh"

//#include "fs/fat/fat32Dentry.hh"
#include "fs/dentrycache.hh"
#include "fs/dentry.hh"

namespace fs{

    namespace ramfs{

        RamFS k_ramfs;
        
        
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


            _root->EntryCreate( "dev", 1 ); // 暂时把第二个参数为正数认为是目录， 还要创建 /dev/sda1, 用来初始化ext4
            _root->EntryCreate( "proc", 1 );
            _root->EntryCreate( "sys", 1 );
            _root->EntryCreate( "tmp", 1 );
            _root->EntryCreate( "mnt", 1 );
            
            
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

            RamFSDen *dentry1;
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "dev", nullptr, _root );
            _root->add_children( dentry1 );
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "proc", nullptr, _root );
            _root->add_children( dentry1 );  

            // fat::Fat32Dentry *dentry2;
            // dentry2 = static_cast<fat::Fat32Dentry *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::FAT32_DENTRY ));
            // dentry2->init( dentry1, nullptr, "fatDentry" );

            RamFSDen *dev = static_cast<RamFSDen *>(_root->EntrySearch( "dev" ));// 准备在dev下创建sda1
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "sda1", nullptr, dev );
            dev->add_children( dentry1 );
            _root->printChildrenInfo( );

            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "sda2", nullptr, dev );
            dev->add_children( dentry1 );

            _root->printChildrenInfo();
            
            dentry1 = static_cast<RamFSDen *>(fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ));
            dentry1->init( "sda3", nullptr, dev );
            dev->add_children( dentry1 );
			return;
        }
    }
}
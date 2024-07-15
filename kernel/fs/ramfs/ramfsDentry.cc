//#include "fs/ramfs/ramfsDentry.hh"
#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"

#include "fs/dentrycache.hh"
#include "fs/inode.hh"
#include "fs/fs.hh"

#include <EASTL/string.h>
#include <EASTL/unordered_map.h>
#include <EASTL/queue.h>
namespace fs{
    
    namespace ramfs{
        
        bool S_ISDIR( uint32 mode ){
            /// @todo check file is directory or not
            /// @todo complement this function, it is not complete,
            //return ( mode & 0xF000 ) == 0x4000;
            return mode > 0;
        }

        Dentry *RamFSDen::EntrySearch( eastl::string name ){
            auto it = children.find( name );
            if( it != children.end() ){
                return it->second;
            }
            return nullptr;
        }

        Dentry* RamFSDen::EntryCreate(eastl::string name, uint32 mode ) {
            if (name.empty()) {
                log_error("RamFSDen::EntryCreate: name is empty");
                return nullptr;
            }

            // check if the name already exists
            if (children.find(name) != children.end()) {
                log_error("RamFSDen::EntryCreate: name already exists");
                return nullptr;
            }
            
            [[maybe_unused]] FileSystem *fs = node->getFS();

            RamFS* nodefs = static_cast <RamFS*>(node->getFS());
            if (nodefs == nullptr) {
                log_error("RamFSDen::EntryCreate: nodefs is not RamFS");
                return nullptr;
            }

            RamInode* inode = new RamInode(nodefs, k_ramfs.alloc_ino(), mode );
            if (inode == nullptr) {
                log_error("RamFSDen::EntryCreate: Failed to create RamInode");
                return nullptr;
            }

            //RamFSDen* dentry = new RamFSDen( name, inode, this);
            RamFSDen *dentry = static_cast<RamFSDen *> (fs::dentrycache::k_dentryCache.alloDentry( DentryType::RAMFS_DENTRY ) );
            dentry->init( name, inode, this );
            if (dentry == nullptr) {
                log_error("RamFSDen::EntryCreate: Failed to create RamFSDen");
                delete inode; // 避免内存泄漏
                return nullptr;
            }

            children [ name ] = dentry;

            log_info("RamFSDen::EntryCreate: created %s, parent %s", name.c_str(), this->name.c_str());
            return dentry;
        }

        Inode *RamFSDen::getNode(){
            return node;
        }

        void RamFSDen::init(uint32 dev, RamFS *fs){
            parent = nullptr;
            node = new RamInode( fs, fs->alloc_ino(), true );
            //node->init( 0, fs, true );
            name = "/";
            isroot = true;
            // init device dentry
            // EntryCreate( "dev", 1 ); // 暂时把第二个参数为正数认为是目录
            // EntryCreate( "proc", 1 );
            // EntryCreate( "sys", 1 );
            // EntryCreate( "tmp", 1 );
            // EntryCreate( "mnt", 1 );
        }

        void RamFSDen::reset( eastl::vector<int> &bitmap ){

            bitmap[ Did ] = 0;
            parent->delete_child( name ); // delete from parent
            parent = nullptr;
            node = nullptr;
            name.clear();
            for( auto &p : children )
            {
                p.second->reset( bitmap );
            }
            children.clear();
            //Did = 0;
        }

        void RamFSDen::printChildrenInfo() {
            eastl::queue<Dentry*> current;
            Dentry *den;
            current.push(this); // 将当前节点添加到队列
            
            while (!current.empty()) {
                den = current.front(); // 获取队列前端的节点
                current.pop(); 
        
                log_info("  %s", den->rName().c_str());
                
                eastl::unordered_map<eastl::string, Dentry*> children = den->getChildren();
                for (auto &child : children) {
                    current.push(child.second); // 将子节点添加到队列中以便后续处理
                }
            }
        }

        eastl::unordered_map< eastl::string, Dentry * >& RamFSDen::getChildren()
        {
            return children;
        }
    }
}
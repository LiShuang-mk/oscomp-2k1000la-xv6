#include "fs/dentrycache.hh"
#include "fs/inode.hh"

#include "fs/fat/fat32inode.hh"

#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"



#include <EASTL/queue.h>

namespace fs
{
    fs::dentry *dentry::EntrySearch(const eastl::string name)
    {
        log_trace("dentry::EntrySearch: name = %s", name.c_str());
        auto it = children.find(name);
        if (it != children.end())
            return it->second;
        if ( [[maybe_unused]] auto subnod = (node->lookup(name)))
        {
            log_trace("dentry::EntrySearch: found inode");
            dentry *subdentry = fs::dentrycache::k_dentryCache.alloDentry();
            subdentry->init(name, (Inode *)subnod, this);
            children[name] = subdentry;
            return subdentry;
        }
        return nullptr;
    }

    fs::dentry *dentry::EntryCreate( eastl::string name, uint32 mode)
    {
        if (name.empty()) {
            log_error("dentry::EntryCreate: name is empty");
            return nullptr;
        }

            // check if the name already exists
        if (children.find(name) != children.end()) {
            log_error("dentry::EntryCreate: name already exists");
            return nullptr;
        }
            
        //[[maybe_unused]] FileSystem *fs = node->getFS();

        Inode* node_ = this->node->mknode( name, mode );
        if (node_ == nullptr) 
        {
            log_error("dentry::EntryCreate: nodefs is not RamFS");
            return nullptr;
        }
        dentry *newden = fs::dentrycache::k_dentryCache.alloDentry();
        newden->init( name, node_, this );

        if ( newden == nullptr ) {
            log_error("dentry::EntryCreate: Failed to create RamFSDen");
            delete node_; // 避免内存泄漏
            return nullptr;
        }

        children [ name ] = newden;

        log_info("RamFSDen::EntryCreate: created %s, parent %s", name.c_str(), this->name.c_str());
        return newden;
    }

    void dentry::reset( eastl::vector<int> &bitmap )
    {
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

    }

    Inode *dentry::getNode()
    {
        return node;
    }

    bool dentry::isRoot()
    {
        return this == node->getFS()->getRoot();
    }

    bool dentry::isMntPoint()
    {
        return this == node->getFS()->getMntPoint();
    }
    void dentry::init( uint32 dev, fat::Fat32SuperBlock *sb, fat::Fat32FS* fs, eastl::string rootname )
    {
        fat::Fat32Inode *node = new fat::Fat32Inode();
        node->init(
            2,
            fs,
            fat::fat32nod_folder,
            0
        );
        rootInit( node, rootname );
        isroot = true;
    }

    void dentry::init( eastl::string name ,Inode *node , dentry *parent )
    {
        this->node = node;
        this->parent = parent;
        this->name = name;
    }
    void dentry::rootInit( Inode *node, eastl::string rootname )
    {
        this->node = node;
        name = rootname;
    }

    void dentry::printChildrenInfo( )
    {
        eastl::queue<dentry*> current;
        dentry *den;
        current.push(this); // 将当前节点添加到队列
            
        while (!current.empty()) {
            den = current.front(); // 获取队列前端的节点
            current.pop(); 
            
            log_info("  %s", den->rName().c_str());            
            
            eastl::unordered_map<eastl::string, dentry*> children = den->getChildren();
            for (auto &child : children) {
                current.push(child.second); // 将子节点添加到队列中以便后续处理
            }
            }
    }

    void dentry::init( uint32 dev, ramfs::RamFS *fs )
    {
        parent = nullptr; 
        node = fs->getSuperBlock()->allocInode( true );
        name = "/";
        isroot = true;  // ramfs 作为根FS进行初始化
    }
}// namespace fs
#include "fs/dentrycache.hh"
#include "fs/inode.hh"

#include "fs/fat/fat32inode.hh"

#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"



#include <EASTL/queue.h>

namespace fs
{
	dentry::~dentry()
	{
		delete _node;
	}

	fs::dentry *dentry::EntrySearch( const eastl::string name )
    {
        log_trace("dentry::EntrySearch: name = %s", name.c_str());
        auto it = children.find(name);
        if (it != children.end())
            return it->second;
        if ( [[maybe_unused]] auto subnod = (_node->lookup(name)))
        {
            log_trace("dentry::EntrySearch: found inode");
            dentry *subdentry = fs::dentrycache::k_dentryCache.alloDentry();
            new ( subdentry ) dentry(name, (Inode *)subnod, this);
            //dentry *subdentry = new dentry(name , subnod, this);
            children[name] = subdentry;
            return subdentry;
        }
        return nullptr;
    }

    fs::dentry *dentry::EntryCreate( eastl::string name, FileAttrs attrs, eastl::string dev_name )
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

        Inode* node_ = this->_node->mknode( name, attrs, dev_name );
        if (node_ == nullptr) 
        {
            log_error("dentry::EntryCreate: nodefs is not RamFS");
            return nullptr;
        }
        //dentry *newden = new dentry( name, node_, this );
        dentry *newden = fs::dentrycache::k_dentryCache.alloDentry();
        new ( newden ) dentry( name, node_, this );
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
		_node = nullptr;
		name.clear();
		for( auto &p : children )
		{
			p.second->reset( bitmap );
		}
		children.clear();

    }

    Inode *dentry::getNode()
    {
        return _node;
    }

    bool dentry::isRoot()
    {
        return this == _node->getFS()->getRoot();
    }

    bool dentry::isMntPoint()
    {
        return this == _node->getFS()->getMntPoint();
    }

    void dentry::printChildrenInfo( )
    {
        eastl::queue<dentry*> current;
        dentry *den;
        FileAttrs attrs;
        current.push(this); // 将当前节点添加到队列
            
        while (!current.empty()) {
            den = current.front(); // 获取队列前端的节点
            current.pop(); 
            attrs = den->getNode()->rMode();

            printf("dentry name is %s, and it's attr is %d, it is a %d file", den->rName().c_str(), attrs.transMode(), attrs.filetype);            
            
            eastl::unordered_map<eastl::string, dentry*> children = den->getChildren();
            for (auto &child : children) {
                current.push(child.second); // 将子节点添加到队列中以便后续处理
            }
            }
    }

    void dentry::unlink()
    {
        /// @todo node->unlink(); linkcnt--;
    }
}// namespace fs
#include "types.hh"
#include "fs/dentry.hh"

#include <EASTL/unordered_map.h>
#include <EASTL/string.h>

namespace fs{

    //class Dentry;

    namespace dentrycache{

        class dentryCache;

    }
    namespace ramfs{

        class RamInode;
        class RamfsSB;
        class RamFS;
        
        class RamFSDen : public Dentry{

            friend class dentrycache::dentryCache;
            eastl::string name;
            eastl::unordered_map<eastl::string, Dentry *> children;
            RamInode *node;
            RamFSDen *parent;
            uint Did; // dentry id
            bool isroot;

            public:
                RamFSDen() = default;
                RamFSDen( eastl::string name_, RamInode *node_ ) : name( name_ ), node( node_ ) {}
                RamFSDen( eastl::string name_, RamInode *node_, RamFSDen *parent_, bool isroot_ = false ) : name( name_ ), node( node_ ), parent( parent_ ), isroot( isroot_ ) {}
                ~RamFSDen() = default;
               
                Dentry *EntrySearch( eastl::string name ) override;
                Dentry *EntryCreate( eastl::string name, uint32 mode ) override;
                Inode *getNode() override;
                bool isRoot() override { return isroot; };
                Dentry *getParent() override { return parent;};
                eastl::string getName() override { return name; };
                bool isMntPoint() override { return false; }; 
                DentryType dentry_type() override { return DentryType::RAMFS_DENTRY; }; //generally ramfs does not have dentry type
                eastl::string rName() override { return name; }; // get dentry' name
                uint getDid() override { return Did; };  // Dentry id should allocated by dentrycache
                void reset( eastl::vector<int> &bitmap ) override ;
                void add_children( Dentry *den) { children[ den->rName() ] = den; };

                void printChildrenInfo();
            public:
                void init( uint32 dev, RamFS *fs ); // for purpose of root init
                void init( eastl::string name_, RamInode *node_, RamFSDen *parent_ ) { name = name_; node = node_; parent = parent_; };
                void delete_child( eastl::string name ) { children.erase( name ); };
                eastl::unordered_map< eastl::string, Dentry * >& getChildren() override ;
        };
    }
}
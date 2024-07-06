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
            eastl::unordered_map<eastl::string, RamFSDen *> children;
            RamInode *node;
            RamFSDen *parent;
            uint Did; // dentry id

            public:
                RamFSDen() = default;
                RamFSDen( eastl::string name_, RamInode *node_ ) : name( name_ ), node( node_ ) {}
                RamFSDen( eastl::string name_, RamInode *node_, RamFSDen *parent_ ) : name( name_ ), node( node_ ), parent( parent_ ) {}
                ~RamFSDen() = default;
               
                Dentry *EntrySearch( eastl::string name ) override;
                Dentry *EntryCreate( eastl::string name, uint32 mode ) override;
                Inode *getNode() override;
                bool isRoot() override { return false; };
                Dentry *getParent() override { return parent;};
                eastl::string getName() override { return name; };
                bool isMntPoint() override { return false; }; 
                DentryType dentry_type() override { return DentryType::RAMFS_DENTRY; }; //generally ramfs does not have dentry type
                eastl::string rName() override { return name; }; // get dentry' name
                uint getDid() override { return Did; };  // Dentry id should allocated by dentrycache
                void reset( eastl::vector<int> &bitmap ) override ;
            public:
                void init( uint32 dev, RamFS *fs ); // for purpose of root init
                void init( eastl::string name_, RamInode *node_, RamFSDen *parent_ ) { name = name_; node = node_; parent = parent_; };
        };
    }
}
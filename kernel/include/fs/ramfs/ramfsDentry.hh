#include "types.hh"
#include "fs/dentry.hh"

#include <EASTL/unordered_map.h>
#include <EASTL/string.h>

namespace fs{

    //class Dentry;

    namespace ramfs{

        class RamInode;
        class RamfsSB;

        class RamFSDen : public Dentry{
            eastl::string name;
            eastl::unordered_map<eastl::string, RamFSDen *> children;
            RamInode *node;
            RamFSDen *parent;

            public:
                RamFSDen( eastl::string name_, RamInode *node_ ) : name( name_ ), node( node_ ) {}
                ~RamFSDen() = default;
                Dentry *EntrySearch( eastl::string name ) override;
                Dentry *EntryCreate( eastl::string name, uint32 mode ) override;
                Inode *getNode() override;
                bool isRoot() override { return false; };
                Dentry *getParent() override { return parent;};
                eastl::string getName() override { return name; };
                bool isMntPoint() override { return false; }; 
                int dentry_type() override { return 0; }; //generally ramfs does not have dentry type
        };
    }
}
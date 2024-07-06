#include "types.hh"
#include "smp/lock.hh"
#include "fs/fat/fat32Dentry.hh"
#include "fs/ramfs/ramfsDentry.hh"

#include <EASTL/string.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/tuple.h>

using eastl::tuple;
using eastl::vector;
using eastl::unique_ptr;
using eastl::list;
using eastl::string;

namespace fs
{
    class Dentry;

    namespace dentrycache
    {

        

        constexpr uint MAX_DENTRY_NUM = 1024;
        constexpr uint DENTRY_TYPES = 2;
        constexpr uint INACTIVE_LIST_MAX_SIZE = 300;  // tentative size

        /**
         * @brief Dentry cache
         * 
         */
        class dentryCache
        {
            smp::Lock _lock;
            list<Dentry *> _active_list;
            list<Dentry *> _inactive_list;
            fat::Fat32Dentry _fatDentryPool[MAX_DENTRY_NUM / DENTRY_TYPES];
            ramfs::RamFSDen _ramDentryPool[MAX_DENTRY_NUM / DENTRY_TYPES];
            vector<int> bitmap;
            
        public:
            dentryCache() = default;
            ~dentryCache() = default;

            void init();
            Dentry *getDentry();
            void putDentry(Dentry *dentry);
            Dentry *alloDentry(DentryType type);
            void releaseDentry(Dentry *dentry);  // who will call this function apparently? , OOOO, it should be releaseInactiveDentry
            void touchDentry(Dentry *dentry);
            Dentry *releaseInactiveDentry( DentryType type );
        };

        extern dentryCache k_dentryCache;
    }
}
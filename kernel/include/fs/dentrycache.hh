#include "types.hh"
#include "fs/dentry.hh"

#include <EASTL/string.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/tuple.h>

#include <smp/spin_lock.hh>

using eastl::tuple;
using eastl::vector;
using eastl::unique_ptr;
using eastl::list;
using eastl::string;

namespace fs
{
    class dentry;
    namespace dentrycache
    {

        

        constexpr uint MAX_DENTRY_NUM = 70;
        constexpr uint DENTRY_TYPES = 2;
        constexpr uint INACTIVE_LIST_MAX_SIZE = 35;  // tentative size
        constexpr uint ACTIVE_LIST_MAX_SIZE = 30;
        /**
         * @brief Dentry cache
         * @test dentryCacheTest
         */
        class dentryCache
        {
            hsai::SpinLock _lock;
            list<dentry *> _active_list;
            list<dentry *> _inactive_list;
            fs::dentry _dentryPool[ MAX_DENTRY_NUM ];
            vector<int> bitmap;
            
        public:
            dentryCache() = default;
            ~dentryCache() = default;

            void init();
            dentry *getDentry();
            void putDentry(dentry *dentry);
            dentry *alloDentry();
            void releaseDentry(dentry *dentry);  // who will call this function apparently? , OOOO, it should be releaseInactiveDentry
            void touchDentry(dentry *dentry);
            void touchDentryInternal(dentry *dentry);
            dentry *releaseInactiveDentry( );

        };

        extern dentryCache k_dentryCache;
    }
}
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

        

        constexpr uint MAX_DENTRY_NUM = 100;
        /**
         * @brief Dentry cache
         * @test dentryCacheTest
         */

        struct dentryCacheElement
        {
            fs::dentry dentry;
            bool is_active_;
            bool pin_;
            dentryCacheElement() { is_active_ = false; pin_ = false;};
            dentryCacheElement( fs::dentry den , bool active_, bool pin_ ) : dentry( den ), is_active_( active_ ), pin_( pin_ ) {}
        };

        class dentryCache
        {
            hsai::SpinLock _lock;
            list<dentryCacheElement *> leafList_;  // leaf list
            list<dentryCacheElement *> intenalList_; // interal list
            list<dentryCacheElement *> freeList_; // free list
            dentryCacheElement *freeListHead_; // free list head
            dentryCacheElement dentryCacheElementPool_[MAX_DENTRY_NUM];
            
        public:
            dentryCache() = default;
            ~dentryCache() = default;

            void init();
            dentry *alloDentry();
            void touchDentry(dentry *dentry);
            dentry *releaseInactiveDentry();
            void intenalListRemove( dentry *dentry );
            void intenalListPushBack( dentry *dentry );
            void leafListPushBack( dentry *dentry );
            void leafListRemove( dentry *dentry );
        };

        extern dentryCache k_dentryCache;
    }
}
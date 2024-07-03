#include "fs/dentrycache.hh"
#include "fs/dentry.hh"

#include <EASTL/string.h>
#include <EASTL/list.h>


namespace fs
{

    namespace dentrycache
    {
        dentryCache k_dentryCache;
        
        void dentryCache::init()
        {
            _lock.init( "dentryCache" );

            // init _fatDentryPool
            for (uint i = 0; i < MAX_DENTRY_NUM / DENTRY_TYPES; ++i) {
                _fatDentryPool[i] = fat::Fat32Dentry();
            }

            //init _ramsDentryPool
            for (uint i = 0; i < MAX_DENTRY_NUM / DENTRY_TYPES; i++)
            {
                _ramDentryPool[i] = ramfs::RamFSDen();
            }

            _active_list.clear();
            _inactive_list.clear();
            bitmap = vector<int>(MAX_DENTRY_NUM, 0);
        }

    } //  namespace dentrycache

} // namespace fs
#include "fs/dentrycache.hh"
#include "fs/dentry.hh"
#include "klib/common.hh"

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
            uint _index = 0;

            // init _fatDentryPool
            for ( ; _index < MAX_DENTRY_NUM / DENTRY_TYPES; ++_index) {
                _fatDentryPool[ _index ] = fat::Fat32Dentry();
                _fatDentryPool[ _index ].Did = _index;
            }

            //init _ramsDentryPool
            for ( ; _index < MAX_DENTRY_NUM; ++_index)
            {
                _ramDentryPool[ _index ] = ramfs::RamFSDen();
                _ramDentryPool[ _index ].Did = _index;  
            }

            _active_list.clear();
            _inactive_list.clear();
            bitmap = vector<int>(MAX_DENTRY_NUM, 0);
        }

        Dentry* dentryCache::alloDentry(DentryType type) 
        {
            _lock.acquire();
            Dentry *dentry = nullptr;
            [[ maybe_unused ]]int found = 0;
            switch (type)
            {
            case DentryType::FAT32_DENTRY:
                for(auto &d : _fatDentryPool)
                {
                    if (bitmap[d.Did] == 0)
                    {
                        found =  1;
                        dentry = &d;
                        bitmap[d.Did] = 1;
                        _active_list.push_back(dentry);
                        break;
                    }
                }
                if( !found ) 
                    /// @todo release in_active_list dentry
                log_error("No available fat32Dentry can be used\n");
                break;
            case DentryType::RAMFS_DENTRY:
                for(auto &d : _ramDentryPool)
                {
                    if (bitmap[d.Did] == 0)
                    {
                        found =  1;
                        dentry = &d;
                        bitmap[d.Did] = 1;
                        _active_list.push_back(dentry);
                        break;
                    }
                }
                if( !found ) 
                    /// @todo release in_active_list dentry
                log_error("No available ramfsDentry can be used\n");
                break;
            default:
                log_error("Dentry type not supported\n");
                break;
            }
            _lock.release();
            return dentry;
        }


        void dentryCache::releaseDentry(Dentry *dentry)
        {
            _lock.acquire();
            _active_list.remove(dentry);  // remove dentry from activelist
            _inactive_list.push_front(dentry);  // add dentry to inactivelist front

            if(_inactive_list.size() > INACTIVE_LIST_MAX_SIZE)
            {
                Dentry *to_release = _inactive_list.back();
                _inactive_list.pop_back();
                /// @todo: reset dentry in dentrycache, and simutaneous reset dentry in fsDnetryTree
                to_release->reset();
            }

            bitmap[dentry->getDid()] = 0;
            _lock.release();
        }

        void dentryCache::touchDentry(Dentry *dentry)
        {
            _lock.acquire();
            // check if dentry is in active_list
            auto it = eastl::find(_inactive_list.begin(), _inactive_list.end(), dentry);
            if (it != _inactive_list.end()) {
                // if dentry is in inactive_list, remove it
                _inactive_list.erase(it);
            } else {
                // if dentry is not in inactive_list, remove it from active_list
                _active_list.remove(dentry);
            }
            // 将dentry添加到active_list的前端
            _active_list.push_front(dentry);
            _lock.release();
        }
    } //  namespace dentrycache

} // namespace fs
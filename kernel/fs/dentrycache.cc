#include "fs/dentrycache.hh"
#include "fs/dentry.hh"
#include "klib/common.hh"

#include <EASTL/string.h>
#include <EASTL/list.h>
#include <EASTL/iterator.h>

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
            for ( uint i = 0; _index < MAX_DENTRY_NUM; ++_index, ++i)
            {
                _ramDentryPool[ i ] = ramfs::RamFSDen();
                _ramDentryPool[ i ].Did = _index;  
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
                        //_active_list.push_back(dentry);
                        touchDentryInternal(dentry);
                        break;
                    }
                }
                if( !found ) /// @todo release in_active_list dentry
                    dentry = releaseInactiveDentry( type );
                //log_error("No available fat32Dentry can be used\n");
                break;
            case DentryType::RAMFS_DENTRY:
                for(auto &d : _ramDentryPool)
                {
                    if (bitmap[d.Did] == 0)
                    {
                        found =  1;
                        dentry = &d;
                        bitmap[d.Did] = 1;
                        //_active_list.push_back(dentry);
                        touchDentryInternal(dentry);  // update dentry's last access time
                        break;
                    }
                }
                if( !found ) 
                    dentry = releaseInactiveDentry( type );
                //log_error("No available ramfsDentry can be used\n");
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
            // 检查 dentry 是否在 active_list 中
            auto it = eastl::find(_active_list.begin(), _active_list.end(), dentry);
            if (it != _active_list.end()) {
                // 如果在 active_list 中，移除并添加到 inactive_list 的前端
                _active_list.erase(it);
                _inactive_list.push_front(dentry);
            } else {
                // 如果不在 active_list 中，尝试从 inactive_list 中释放
                auto it_inactive = eastl::find(_inactive_list.begin(), _inactive_list.end(), dentry);
                if (it_inactive != _inactive_list.end()) {
                    ( *it_inactive )->reset(bitmap);
                    _inactive_list.erase(it_inactive); // 从 inactive_list 中移除
                }
                bitmap[dentry->getDid()] = 0; // 更新 bitmap 状态
            }
            //releaseDentry( dentry->getParent() );
            // 更新 bitmap 状态
            //bitmap[dentry->getDid()] = 0;
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
            if(_active_list.size() >= ACTIVE_LIST_MAX_SIZE)
            {
                Dentry *to_release = _active_list.back();
                releaseDentry(to_release);
            }
            _active_list.push_front(dentry);
            touchDentryInternal(dentry->getParent());
            _lock.release();
        }
        
        void dentryCache::touchDentryInternal(Dentry *dentry)
        {
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
            if(_active_list.size() >= ACTIVE_LIST_MAX_SIZE)
            {
                Dentry *to_release = _active_list.back();
                releaseDentry(to_release);
            }
            _active_list.push_front(dentry);
            if(dentry->getParent() != nullptr)// 这里是不对劲的，因为touchdentryintenal调用方是allocatedentry，这时候分配的dentry的parent都是空的，不能递归的touch他的双亲
                touchDentryInternal(dentry->getParent());
        }

        Dentry* dentryCache::releaseInactiveDentry(DentryType type) {
            for (auto it = _inactive_list.rbegin(); it != _inactive_list.rend(); ++it) {
                if ((*it)->dentry_type() == type) {
                    // 找到了匹配的dentry
                    Dentry* matchedDentry = *it;
                    //auto eraseIt = eastl::next(it).base();
                    //_inactive_list.erase(eraseIt);
                    //_lock.release(); // 释放锁
                    releaseDentry(matchedDentry); // 释放dentry
                    return matchedDentry; // 返回找到的dentry
                }
            }
            return nullptr; // 如果没有找到匹配的dentry，返回nullptr
        }
    } //  namespace dentrycache

} // namespace fs
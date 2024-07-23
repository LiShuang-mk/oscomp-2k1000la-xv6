#include "fs/dentrycache.hh"
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

            for( ; _index < MAX_DENTRY_NUM ; ++_index )
            {
                // _dentryPool[ _index ] =  dentry();
                _dentryPool[ _index ].Did = _index;
            }
            _active_list.clear();
            _inactive_list.clear();
            bitmap = vector<int>(MAX_DENTRY_NUM, 0);
        }

        dentry* dentryCache::alloDentry( ) 
        {
            _lock.acquire();
            dentry *dentry = nullptr;
            [[ maybe_unused ]]int found = 0;
 
            for( auto &it: _dentryPool )
            {
                if( bitmap[it.Did] == 0 )
                {
                    found = 1;
                    dentry = &it;
                    bitmap[it.Did] = 1;
                    _active_list.push_back(dentry);
                    break;
                }
            }
            if( !found ) 
            {
                dentry = releaseInactiveDentry( );
            }
            _lock.release();
            return dentry;
        }


        void dentryCache::releaseDentry(dentry *den)
        {
            // 检查 dentry 是否在 active_list 中
            auto it = eastl::find(_active_list.begin(), _active_list.end(), den);
            if (it != _active_list.end()) {
                // 如果在 active_list 中，移除并添加到 inactive_list 的前端
                _active_list.erase(it);
                _inactive_list.push_front(den);
            } else {
                // 如果不在 active_list 中，尝试从 inactive_list 中释放
                auto it_inactive = eastl::find(_inactive_list.begin(), _inactive_list.end(), den);
                if (it_inactive != _inactive_list.end()) {
                    ( *it_inactive )->reset(bitmap);
                    _inactive_list.erase(it_inactive); // 从 inactive_list 中移除
                }
                bitmap[den->getDid()] = 0; // 更新 bitmap 状态
            }
            //releaseDentry( dentry->getParent() );
            // 更新 bitmap 状态
            //bitmap[dentry->getDid()] = 0;
        }

        void dentryCache::touchDentry(dentry *den)
        {
            _lock.acquire();
            // check if dentry is in active_list
            auto it = eastl::find(_inactive_list.begin(), _inactive_list.end(), den);
            if (it != _inactive_list.end()) {
                // if dentry is in inactive_list, remove it
                _inactive_list.erase(it);
            } else {
                // if dentry is not in inactive_list, remove it from active_list
                _active_list.remove(den);
            }
            // 将dentry添加到active_list的前端
            if(_active_list.size() >= ACTIVE_LIST_MAX_SIZE)
            {
                dentry *to_release = _active_list.back();
                releaseDentry(to_release);
            }
            _active_list.push_front(den);
            if( den->getParent() != nullptr )
                touchDentryInternal(den->getParent());
            _lock.release();
        }
        
        void dentryCache::touchDentryInternal(dentry *den)
        {
            // check if dentry is in active_list
            auto it = eastl::find(_inactive_list.begin(), _inactive_list.end(), den);
            if (it != _inactive_list.end()) {
                // if dentry is in inactive_list, remove it
                _inactive_list.erase(it);
            } else {
                // if dentry is not in inactive_list, remove it from active_list
                _active_list.remove(den);
            }
            // 将dentry添加到active_list的前端
            if(_active_list.size() >= ACTIVE_LIST_MAX_SIZE)
            {
                dentry *to_release = _active_list.back();
                releaseDentry(to_release);
            }
            _active_list.push_front(den);
            if(den->getParent() != nullptr)// 这里是不对劲的，因为touchdentryintenal调用方是allocatedentry，这时候分配的dentry的parent都是空的，不能递归的touch他的双亲
                touchDentryInternal(den->getParent());
        }

        dentry* dentryCache::releaseInactiveDentry( ) {
            for (auto it = _inactive_list.rbegin(); it != _inactive_list.rend(); ++it) {
                    // 找到了匹配的dentry
                    dentry* matchedDentry = *it;
                    //auto eraseIt = eastl::next(it).base();
                    //_inactive_list.erase(eraseIt);
                    //_lock.release(); // 释放锁
                    releaseDentry(matchedDentry); // 释放dentry
                    bitmap[matchedDentry->getDid()] = 1;
                    return matchedDentry; // 返回找到的dentry
            }
            return nullptr; // 如果没有找到匹配的dentry，返回nullptr
        }
    } //  namespace dentrycache

} // namespace fs
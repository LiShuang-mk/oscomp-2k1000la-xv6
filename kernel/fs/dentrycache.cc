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
                dentryCacheElementPool_[ _index ].is_active_ = false;
                dentryCacheElementPool_[ _index ].pin_ = false;
                freeList_.push_back( &dentryCacheElementPool_[ _index ] );
            }
            leafList_.clear();
            intenalList_.clear();
        }

        dentry* dentryCache::alloDentry( ) 
        {
            _lock.acquire();
            dentry *dentry = nullptr;
 
            if( freeList_.empty() )
            {
                dentry = releaseInactiveDentry( );
            }
            else
            {
                dentryCacheElement *element = freeList_.front();
                freeList_.pop_front();
                dentry = &( element->dentry );
                leafListPushBack( dentry );
            }
            _lock.release();
            return dentry;
        }

        void dentryCache::touchDentry(dentry *den)
        {
            for( auto &it : leafList_ )
            {
                if( &( it->dentry ) == den )
                {
                    it->pin_ = true;
                    leafList_.remove( it );
                    intenalList_.push_back( it );
                }
            }
        }
        
        dentry* dentryCache::releaseInactiveDentry( ) 
        {
            for( auto &it : leafList_ )
            {
                if( it->pin_ == false ) // leaf的pin恒为 false
                {
                    // 处理 其 父节点
                    dentry *parent = it->dentry.getParent();
                    if( parent )
                    {
                        parent->children.erase( it->dentry.rName() );
                        if( parent->children.empty() )
                        {
                            intenalListRemove( parent );
                            leafListPushBack( parent );
                        }    
                    }
                    it->is_active_ = false;
                    leafList_.remove( it );
                    return &( it->dentry );
                }
            }
            return nullptr; // 如果没有找到匹配的dentry，返回nullptr
        }

        void dentryCache::intenalListRemove( dentry * d ) 
        {
            for(auto &it : intenalList_)
            {
                if( &( it->dentry ) == d )
                {
                    intenalList_.remove( it );
                    return;
                }
            }
        }

        void dentryCache::intenalListPushBack( dentry * d )
        {
            dentryCacheElement element = dentryCacheElement( *d, true, true );
            intenalList_.push_back( &element );
            return;
        }

        void dentryCache::leafListRemove( dentry * d )
        {
            for(auto &it : leafList_)
            {
                if( &( it->dentry ) == d )
                {
                    leafList_.remove( it );
                    return;
                }
            }
        }

        void dentryCache::leafListPushBack( dentry * d )
        {
            dentryCacheElement *element = new dentryCacheElement( *d, false, false );
            leafList_.push_back( element );
            return;
        }

    } //  namespace dentrycache

} // namespace fs
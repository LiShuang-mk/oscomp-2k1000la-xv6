
#include "pm/sharemem.hh"


namespace pm
{
    Shm Shmtabs[ SHM_NUM ];
    
    void Shm::init(const char* _lock_name)
    {
        _lock.init(_lock_name);
        refcnt = 0;
        pagenum = 0;
        for (uint i = 0; i < MAX_SHM_PGNUM; i++)
        {
            phyaddr[i] = nullptr;
        }
    }

}
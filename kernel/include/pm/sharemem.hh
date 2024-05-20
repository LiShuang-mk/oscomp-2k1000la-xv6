
#pragma once
#include "smp/lock.hh"

namespace pm
{
    class ShmManager;

    constexpr uint SHM_NUM = 10;
	constexpr uint MAX_SHM_PGNUM = 4;
    class Shm
    {
        friend ShmManager;
        
        private:
            smp::Lock _lock;
            int refcnt;     // reference count
            int pagenum;    // number of pages
            void *phyaddr[ pm::MAX_SHM_PGNUM ];   // physical address

        public:
            Shm() {};
            void init(const char* _lock_name);
    };    
    
    extern Shm Shmtabs[ pm::SHM_NUM ];
}
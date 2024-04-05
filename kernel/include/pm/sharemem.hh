
#pragma once
#include "smp/lock.hh"
#include "pm/shmmanager.hh"
#define SHM_NUM 8
#define MAX_SHM_PGNUM 4

namespace pm
{
    class Shm
    {
        friend ShmManager;
        
        private:
            smp::Lock _lock;
            int refcnt;     // reference count
            int pagenum;    // number of pages
            void *phyaddr[MAX_SHM_PGNUM];   // physical address

        public:
            Shm() {};
            void init(const char* _lock_name);
    };    
    
    extern Shm Shmtabs[ SHM_NUM ];
}
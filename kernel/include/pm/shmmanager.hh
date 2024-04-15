
#pragma once
#include "pm/sharemem.hh"
#include "mm/page_table.hh"
namespace pm
{
    class ShmManager
    {   
        private:
            
        public:
            ShmManager() = default;
            void init(const char* _lock_name);
            int shmkeyused(uint key, uint mask);
            void *shmgetat(uint key,  uint num);
            int shmadd(uint key, uint pgnum, void *phyaddr[MAX_SHM_PGNUM]);
            void shmaddcnt(uint mask);
            int shmrefcnt(uint key);
            int shmrm(uint key);
            int shmrelease(mm::PageTable &pt, uint64 shm, uint keymask);
    };    

    extern ShmManager k_shmManager;
}
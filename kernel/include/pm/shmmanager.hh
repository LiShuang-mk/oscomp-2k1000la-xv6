
#pragma once
#include "smp/lock.hh"
#include "pm/sharemem.hh"
#include "mm/page.hh"
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
            void shmaddcnt(uint key);
            int shmrefcnt(uint key);
            int shmrm(uint key);
    };    

    extern ShmManager k_shmManager;
}
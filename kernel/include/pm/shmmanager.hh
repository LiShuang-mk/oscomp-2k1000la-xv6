
#pragma once
#include "smp/lock.hh"

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
            void *shmat(int shmid);
    };    

    extern ShmManager k_shmManager;
}
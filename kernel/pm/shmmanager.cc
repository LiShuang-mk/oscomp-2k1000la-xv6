#include "pm/shmmanager.hh"
#include "mm/page_table.hh"
#include "pm/process.hh"
#include "pm/sharemem.hh"

namespace pm{

    ShmManager k_shmManager;

    void ShmManager::init(const char* _lock_name)
    {
        for (int i = 0; i < SHM_NUM; i++)
        {
            Shm &shm = Shmtabs[i];
            shm.init(_lock_name);
        }
    }

    int ShmManager::shmkeyused(uint key, uint mask)
    {
        if(key < 0 || key >= SHM_NUM)
        {
            return -1;
        }
        return (mask >> key) & 1;
    }

    void *ShmManager::shmgetat(uint key, uint num)
    {
        // mm::PageTable pg;
        // pm::Pcb *pcb;
        // void *phyaddr[MAX_SHM_PGNUM];
        // uint64 shm = 0;
        // if(key < 0 || key >= SHM_NUM || num < 0 || num >= MAX_SHM_PGNUM)
        // {
        //     return nullptr;
        // }
        // Shmtabs[key]._lock.acquire();
        // pcb = pm::ProcessManager::get_cur_pcb();  
        // pg = pcb->_pt;  // get page table

        // // case 1 : if the proc has held the share memory, return address directly
        // if(pcb->_shmkeymask & (1 << key))
        // {
        //     Shmtabs[key]._lock.release();
        //     return pcb->_shmva[key];
        // }

        // // case 2: if sys have not create key's share memory, create it and map it to the proc
        // if(Shmtabs[key].refcnt == 0)
        // {
        //     //shm = mm::k_mm.alloc_pages(MAX_SHM_PGNUM);
        // }

        return nullptr;
    }
}
#include "pm/shmmanager.hh"
#include "mm/virtual_memory_manager.hh"
#include "pm/process_manager.hh"
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
        mm::PageTable pg;
        pm::Pcb *pcb;//shm = mm::k_mm.alloc_pages(MAX_SHM_PGNUM);
        void *phyaddr[MAX_SHM_PGNUM];
        uint64 shm = 0;
        if(key < 0 || key >= SHM_NUM || num < 0 || num >= MAX_SHM_PGNUM)
        {
            return nullptr;
        }
        Shmtabs[key]._lock.acquire();
        pcb = pm::k_pm.get_cur_pcb();
        pg = pcb->_pt;  // get page table

        // case 1 : if the proc has held the share memory, return address directly
        if(pcb->_shmkeymask & (1 << key))
        {
            Shmtabs[key]._lock.release();
            return pcb->_shmva[key];
        }

        // case 2: if sys have not create key's share memory, create it and map it to the proc
        if(Shmtabs[key].refcnt == 0)
        {
            shm = mm::k_vmm.allocshm(pg,shm,shm - num * mm::pg_size, pcb->_sz, phyaddr);
            if(shm == 0)
            {
                Shmtabs[key]._lock.release();
                return reinterpret_cast<void*>(-1);
            }
            pcb->_shmva[key] = (void *)shm;
            shmadd(key, num, phyaddr);
        }

        // case 3: proc don't hold the share memory, but sys have created it, map it to the proc
        else
        {
            for(uint i = 0; i < num; i++)
            {
                phyaddr[i] = Shmtabs[key].phyaddr[i];
            }
            num = Shmtabs[key].pagenum;

            if((shm = mm::k_vmm.mapshm(pg, shm, shm - num * mm::pg_size, pcb->_sz, phyaddr)) == 0)
            {
                Shmtabs[key]._lock.release();
                return reinterpret_cast<void*>(-1);
            }
            pcb->_shmva[key] = (void *)shm;
            Shmtabs[key].refcnt++;
        }
        pcb->_shm = shm;   
        pcb->_shmkeymask |= (1 << key);
        Shmtabs[key]._lock.release();
        return (void *)shm;
    }

    int ShmManager::shmadd(uint key, uint pgnum, void *phyaddr[MAX_SHM_PGNUM])
    {
        if(key < 0 || key >= SHM_NUM || pgnum < 0 || pgnum >= MAX_SHM_PGNUM)
        {
            return -1;
        }
        Shmtabs[key].refcnt = 1;
        Shmtabs[key].pagenum = pgnum;
        for(uint i = 0; i < pgnum; i++)
        {
            Shmtabs[key].phyaddr[i] = phyaddr[i];
        }
        return 0;
    }

    void ShmManager::shmaddcnt(uint key)
    {
        if(key < 0 || key >= SHM_NUM)
        {
            return;
        }
        Shmtabs[key].refcnt++;
    }

    int ShmManager::shmrefcnt(uint key)
    {
        if(key < 0 || key >= SHM_NUM)
        {
            return -1;
        }

        Shmtabs[key]._lock.acquire();
        int cnt;
        cnt = Shmtabs[key].refcnt;
        Shmtabs[key]._lock.release();
        return cnt;
    }

    int ShmManager::shmrm(uint key)
    {
        if(key < 0 || key >= SHM_NUM)
        {
            return -1;
        }
        Shm *shmem = &Shmtabs[key];
        for(int i = 0; i < shmem->pagenum; i++)
        {
            mm::k_pmm.free_page(shmem->phyaddr[i]);
        }
        shmem->refcnt = 0;
        return 0;
    }
}

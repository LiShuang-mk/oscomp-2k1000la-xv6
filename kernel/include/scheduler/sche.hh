#pragma once
#include "smp/lock.hh"
#include "pm/process.hh"

namespace pm
{

    class Scheduler{
        private:
            smp::Lock _sche_lock;
        public:
            Scheduler() = default;
            void init(const char *name);
            void add_thread();
            void remove_thread();
            int  get_highest_proirity();
            void schedule();
            void switch_to_proc(pm::Pcb *p);
    };

    extern Scheduler k_scheduler;
}
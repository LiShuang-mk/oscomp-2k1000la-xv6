#include "scheduler/sche.hh"
#include "smp/lock.hh"
#include "pm/process_manager.hh"
#include "hal/cpu.hh"

namespace sche{
    
    Scheduler k_scheduler;

    void Scheduler::init(const char *name){  //在考虑这个🔓要不要丢了算了，没想到作用是什么
        _sche_lock.init(name);
    }

    void Scheduler::switch_to_proc(pm::Pcb *p){
        loongarch::Cpu *c = loongarch::Cpu::get_cpu();
        p->get_lock().acquire();
        if(p->get_state() == pm::ProcState::runnable)
        {
            pm::k_pm.change_state(p, pm::ProcState::running);
            loongarch::k_cpus->set_cur_proc(p);
            pm::Context& context_ref = p->get_context();
            loongarch::Context& loongarch_context_ref = reinterpret_cast<loongarch::Context&>(context_ref);  //应该把pm和loongarch中的context删除一个的
            swtch(c->get_context(), &loongarch_context_ref);
            loongarch::k_cpus->set_cur_proc(&pm::k_proc_pool[0]);
        }
        p->get_lock().release();
    }

    int Scheduler::get_highest_proirity(){
        _sche_lock.acquire();
        int _priority = 19;
        for(pm::Pcb &p : pm::k_proc_pool){    
            if(p.get_priority() < _priority && p.get_state() == pm::ProcState::runnable){
                _priority = p.get_priority();
            }
        }
        _sche_lock.release();
        return _priority;
    }

    void  Scheduler::schedule(){
        pm::Pcb *p;
        //loongarch::Cpu *c = loongarch::Cpu::get_cpu();
        int priority;
        int needed = 1;

        loongarch::k_cpus->set_cur_proc(&pm::k_proc_pool[0]);

        for(;;)
        {
            loongarch::k_cpus->interrupt_on();

            for(p = pm::k_proc_pool;p < &pm::k_proc_pool[pm::num_process]; p++)
            {
                if(needed)
                {
                    priority = get_highest_proirity();
                }

                needed = 0;
                if(p->get_state() != pm::ProcState::runnable || p->get_priority() > priority)
                {
                    continue;
                }
                switch_to_proc(p);
                needed = 1;
            }
        }
    }

}
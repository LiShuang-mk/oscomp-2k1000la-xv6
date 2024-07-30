#include "pm/ipc/signal.hh"
#include "pm/process_manager.hh"

namespace pm
{
    namespace ipc
    {
        namespace signal
        {
            int sigAction( int flag, sigaction * newact, sigaction * oldact )
            {
                if( flag <=0 || flag >= SIGRTMAX || flag == SIGKILL || flag == SIGQUIT )
                    return -1; 
                pm::Pcb * cur_proc = pm::k_pm.get_cur_pcb();
                if( oldact != nullptr )
                    oldact = cur_proc->_sigactions[ flag - 1 ];
                if( newact != nullptr )
                    cur_proc->_sigactions[ flag - 1] = newact;
                return 0;
            }
        }
    }
}

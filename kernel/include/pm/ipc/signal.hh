#include "types.hh"



namespace pm
{
    namespace ipc
    {
        namespace signal
        {
        #include <asm-generic/signal.h>
        #include <asm/sigcontext.h>

        int sigAction( int flag, sigaction * newact, sigaction * oldact );

        int sigprocmask( int how, sigset_t* newset, sigset_t* oldset, size_t sigsize);
        }
    }
}
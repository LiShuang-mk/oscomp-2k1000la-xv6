#include "fs/file/file.hh"

#include "pm/process.hh"
#include "pm/process_manager.hh"

#include "klib/klib.hh"
#include "types.hh"

#include <EASTL/string.h>

namespace fs
{
    int file::readlink( uint64 buf, size_t size )
    {
        pm::Pcb *cur_proc = pm::k_pm.get_cur_pcb();

        dentry * cwd_ = cur_proc->_cwd;
        eastl::string abs_path = cur_proc->_name;
        eastl::string temp;
        while( cwd_ )
        {
            temp = cwd_->getParent()->rName() + "/";
            abs_path = temp + cwd_->rName();
            cwd_ = cwd_->getParent();
        }

        int ret;
        size < abs_path.length() ? ret = size : ret = abs_path.length();

        memcpy( (void *)buf, abs_path.c_str(), ret);
        return ret;
    }
}
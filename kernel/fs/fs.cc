#include "fs/fs.hh"
#include "klib/common.hh"
#include <EASTL/string.h>
#include "pm/process_manager.hh"
#include <EASTL/unordered_map.h>


namespace fs{
    eastl::unordered_map<eastl::string, FileSystem *> mnt_table;

    Dentry* Dentry::EntrySearch(Dentry *self, eastl::string name){
        if(children.find(name) != children.end())
            return children[name];
        
        if(auto subnode = node->lookup(name))
        {
            auto subDEntry = new Dentry(self, subnode, name);
            children.insert({name, subDEntry});
            return subDEntry;
        }
        return nullptr;
    }

    Dentry* Dentry::EntryCreate(Dentry *self, eastl::string name, mode_t mode){
        if(children.find(name) != children.end()){
            log_error("File %s already exists!\n", name.c_str());
            return nullptr;
        }

        if(auto subnode = node->mknode(name, mode))
        {
            auto subDEntry = new Dentry(self, subnode, name);
            children.insert({name, subDEntry});
            return subDEntry;
        }
        return nullptr;
    }

    int File::write(void *buf, size_t len){
        int RC = -1;
        if(!ops.fields.w){
            log_error("File is not allowed to write!\n");
            return RC;
        }
        log_info("write %d bytes...\n",len);

        switch (data.get_Type())
        {
            case FileTypes::FT_STDOUT:
            case FileTypes::FT_STDERR:
                printf("%s", (char *)buf);
                RC = len;
                break;
            case FileTypes::FT_ENTRY:
            {
                uint64* buf_uint64 = static_cast<uint64*>(buf);
                uint64 buffer = *buf_uint64;
                if(data.get_Entry()->getNode()->nodWrite(buffer,data.get_off(),len) == len){
                    data.get_off() += len;
                    RC = len;
                }
                break;
            }
            default: 
                log_panic("file::write(),Unknown File Type!\n");
                break;
        }

        return RC;
    }

    int File::read(void *buf, size_t len, int off_, bool update){
        int RC = -1;
        if(!ops.fields.r){
            log_error("File is not allowed to read!\n");
            return RC;
        }
        switch (data.get_Type())
        {
            case FileTypes::FT_STDIN:
                log_error("stdin is not allowed to read!\n");
                break;
            case FileTypes::FT_DEVICE:
                log_error("Device is not allowed to read!\n");
                break;
            case FileTypes::FT_ENTRY:
            {
                if(off_ < 0)
                    off_ = data.get_off();
                uint64* buf_uint64 = static_cast<uint64*>(buf);
                uint64 buffer = *buf_uint64;
                if(auto read_len = data.get_Entry()->getNode()->nodeRead(buffer,off_,len)){
                    if(update)
                        data.get_off() += read_len;
                    RC = read_len;
                }
                break;
            }
            default: 
                log_panic("file::read(),Unknown File Type!\n");
                break;
        }
        return RC;
    }

    void Path::pathbuild()
    {
        if(pathname.size() < 1) { base = pm::k_pm.get_cur_pcb()->get_cwd(); return; }
        else if(pathname[0] == '/') { base = mnt_table["/"]->getSuperBlock()->getRoot(); }
        else if(base == nullptr) { base = pm::k_pm.get_cur_pcb()->get_cwd(); }
        
        size_t len = pathname.size();
        if(len > 0) {  // 保证数组长度不为0
            auto ind = new size_t[len][2] { { 0, 0 } };
            bool rep = true;
            int dirnum = 0;
            for(size_t i = 0; i < len; ++i) {  // 识别以'/'结尾的目录
                if(pathname[i] == '/') {
                    if(!rep) {
                        rep = true;
                        ++dirnum;
                    }
                }
                else {
                    ++(ind[dirnum][1]);
                    if(rep) {
                        rep = false;
                        ind[dirnum][0] = i;
                    }
                }
            }
            if(!rep) { ++dirnum; }  // 补齐末尾'/'
            dirname = eastl::vector<eastl::string>(dirnum);
            for(size_t i = 0; i < (size_t)dirnum; ++i) { dirname[i] = pathname.substr(ind[i][0], ind[i][1]); }
            delete[] ind;
        }
        return;
    }
};

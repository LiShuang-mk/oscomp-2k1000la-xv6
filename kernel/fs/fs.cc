#include "fs/fs.hh"
#include "klib/common.hh"


namespace fs{
    Dentry* Dentry::EntrySearch(Dentry *self, char *name){
        for(int i=0;i<10;i++)
            if(children[i]->name == name)
                return children[i];
        
        if(auto subnode = node->lookup(name))
        {
            auto subDEntry = new Dentry(self, subnode, name);
            children[point++] = subDEntry;
            return subDEntry;
        }
        return nullptr;
    }

    Dentry* Dentry::EntryCreate(Dentry *self, char *name, uint32 mode){
        for(int i=0;i<10;i++)
            if(children[i]->name == name){
                log_warn("Couldn't create DeEntry %s, because there is a same one exits!\n", name);
                return children[i];
            }

        if(auto subnode = node->mknode(name, mode))
        {
            auto subDEntry = new Dentry(self, subnode, name);
            children[point++] = subDEntry;
            return subDEntry;
        }
        return nullptr;
    }

    int File::write(void *buf, uint32 len){
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

    int File::read(void *buf, uint32 len, int off_, bool update){
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
};

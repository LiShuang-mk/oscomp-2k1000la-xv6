// Format of an ELF executable file
#include "types.hh"


namespace elf{
    enum elfEnum{
        ELF_MAGIC = 0x464C457FU,  // "\x7FELF" in little endian
        ELF_PROG_LOAD = 1,
        ELF_PROG_FLAG_EXEC = 1,
        ELF_PROG_FLAG_WRITE = 2,
        ELF_PROG_FLAG_READ = 4,
        ELF_GNU_STACK = 0x6474e551U,
    };

    // File header
    struct elfhdr {
        uint magic;  // must equal ELF_MAGIC
        uchar elf[12];
        ushort type;
        ushort machine;
        uint version;
        uint64 entry;
        uint64 phoff;
        uint64 shoff;
        uint flags;
        ushort ehsize;
        ushort phentsize;
        ushort phnum;
        ushort shentsize;
        ushort shnum;
        ushort shstrndx;
    };

    // Program section header
    struct proghdr {
        uint32 type;  
        uint32 flags;
        uint64 off;
        uint64 vaddr;  
        uint64 paddr;
        uint64 filesz;
        uint64 memsz;
        uint64 align;
    };


}





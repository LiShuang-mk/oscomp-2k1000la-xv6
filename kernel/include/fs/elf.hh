// Format of an ELF executable file
#include "types.hh"


namespace elf
{
	enum elfEnum
	{
		ELF_MAGIC = 0x464C457FU,  // "\x7FELF" in little endian
		ELF_PROG_LOAD = 1,
		ELF_PROG_FLAG_EXEC = 1,
		ELF_PROG_FLAG_WRITE = 2,
		ELF_PROG_FLAG_READ = 4,
		ELF_GNU_STACK = 0x6474e551U,
	};

	// File header
	struct elfhdr
	{
		uint magic;  // must equal ELF_MAGIC
		uchar elf[ 12 ];
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
	struct proghdr
	{
		uint32 type;
		uint32 flags;
		uint64 off;
		uint64 vaddr;
		uint64 paddr;
		uint64 filesz;
		uint64 memsz;
		uint64 align;
	};

	// from glibc elf.h
	struct Elf64_auxv_t
	{
		uint64 a_type;		/* Entry type */
		union
		{
			uint64 a_val;		/* Integer value */
			/* We use to have pointer elements added here.  We cannot do that,
		   though, since it does not work when using 32-bit definitions
		   on 64-bit platforms and vice versa.  */
		} a_un;
	};

	enum AuxvEntryType
	{

	/* Legal values for a_type (entry type).  */

		AT_NULL = 0,		/* End of vector */
		AT_IGNORE = 1,		/* Entry should be ignored */
		AT_EXECFD = 2,		/* File descriptor of program */
		AT_PHDR = 3,		/* Program headers for program */
		AT_PHENT = 4,		/* Size of program header entry */
		AT_PHNUM = 5,		/* Number of program headers */
		AT_PAGESZ = 6,		/* System page size */
		AT_BASE = 7,		/* Base address of interpreter */
		AT_FLAGS = 8,		/* Flags */
		AT_ENTRY = 9,		/* Entry point of program */
		AT_NOTELF = 10,		/* Program is not ELF */
		AT_UID = 11,		/* Real uid */
		AT_EUID = 12,		/* Effective uid */
		AT_GID = 13,		/* Real gid */
		AT_EGID = 14,		/* Effective gid */
		AT_CLKTCK = 17,		/* Frequency of times() */

	/* Some more special a_type values describing the hardware.  */
		AT_PLATFORM = 15,		/* String identifying platform.  */
		AT_HWCAP = 16,			/* Machine-dependent hints about
									processor capabilities.  */

	/* This entry gives some information about the FPU initialization
		performed by the kernel.  */

		AT_FPUCW = 18,			/* Used FPU control word.  */

	/* Cache block sizes.  */
		AT_DCACHEBSIZE = 19,	/* Data cache block size.  */
		AT_ICACHEBSIZE = 20,	/* Instruction cache block size.  */
		AT_UCACHEBSIZE = 21,	/* Unified cache block size.  */

	/* A special ignored value for PPC, used by the kernel to control the
		interpretation of the AUXV. Must be > 16.  */

		AT_IGNOREPPC = 22,		/* Entry should be ignored.  */

		AT_SECURE = 23,			/* Boolean, was execve setuid-like?  */

		AT_BASE_PLATFORM = 24,	/* String identifying real platforms.*/

		AT_RANDOM = 25,			/* Address of 16 random bytes.  */

		AT_HWCAP2 = 26,			/* More machine-dependent hints about
									processor capabilities.  */

		AT_RSEQ_FEATURE_SIZE = 27,	/* rseq supported feature size.  */
		AT_RSEQ_ALIGN = 28,			/* rseq allocation alignment.  */

		AT_EXECFN = 31,			/* Filename of executable.  */

	/* Pointer to the global system page used for system calls and other
		nice things.  */
		
		AT_SYSINFO = 32,
		AT_SYSINFO_EHDR = 33,

	/* Shapes of the caches.  Bits 0-3 contains associativity; bits 4-7 contains
		log2 of line size; mask those to get cache size.  */

		AT_L1I_CACHESHAPE = 34,
		AT_L1D_CACHESHAPE = 35,
		AT_L2_CACHESHAPE = 36,
		AT_L3_CACHESHAPE = 37,

	/* Shapes of the caches, with more room to describe them.
		*GEOMETRY are comprised of cache line size in bytes in the bottom 16 bits
		and the cache associativity in the next 16 bits.  */

		AT_L1I_CACHESIZE = 40,
		AT_L1I_CACHEGEOMETRY = 41,
		AT_L1D_CACHESIZE = 42,
		AT_L1D_CACHEGEOMETRY = 43,
		AT_L2_CACHESIZE = 44,
		AT_L2_CACHEGEOMETRY = 45,
		AT_L3_CACHESIZE = 46,
		AT_L3_CACHEGEOMETRY = 47,

		AT_MINSIGSTKSZ = 51, /* Stack needed for signal delivery  */
	};

}





/* p_elf.h --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2006 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2006 Laszlo Molnar
   All Rights Reserved.

   UPX and the UCL library are free software; you can redistribute them
   and/or modify them under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer   Laszlo Molnar
   markus@oberhumer.com      ml1050@users.sourceforge.net
 */


#ifndef __UPX_P_ELF_H
#define __UPX_P_ELF_H


/*************************************************************************
// N_Elf
**************************************************************************/

namespace N_Elf {

template <class THalf, class TWord, class TXword, class TAddr, class TOff>
struct ElfITypes
{
    // integral types
    typedef THalf   Half;
    typedef TWord   Word;
    typedef TXword  Xword;
    typedef TAddr   Addr;
    typedef TOff    Off;
    typedef THalf   Section;
    typedef THalf   Versym;
};


// The ELF file header. This appears at the start of every ELF file.
template <class TElfITypes>
struct Ehdr
{
    typedef typename TElfITypes::Half    Half;
    typedef typename TElfITypes::Word    Word;
    typedef typename TElfITypes::Addr    Addr;
    typedef typename TElfITypes::Off     Off;

    unsigned char e_ident[16];  /* Magic number and other info */
    Half e_type;                /* Object file type */
    Half e_machine;             /* Architecture */
    Word e_version;             /* Object file version */
    Addr e_entry;               /* Entry point virtual address */
    Off  e_phoff;               /* Program header table file offset */
    Off  e_shoff;               /* Section header table file offset */
    Word e_flags;               /* Processor-specific flags */
    Half e_ehsize;              /* ELF header size in bytes */
    Half e_phentsize;           /* Program header table entry size */
    Half e_phnum;               /* Program header table entry count */
    Half e_shentsize;           /* Section header table entry size */
    Half e_shnum;               /* Section header table entry count */
    Half e_shstrndx;            /* Section header string table index */

    enum { // e_ident[]
        EI_CLASS      = 4,
        EI_DATA       = 5,      /* Data encoding */
        EI_VERSION    = 6,
        EI_OSABI      = 7,
        EI_ABIVERSION = 8,
    };
    enum { // e_ident[EI_CLASS]
        ELFCLASS32 = 1,         /* 32-bit objects */
        ELFCLASS64 = 2,         /* 64-bit objects */
    };
    enum { // e_ident[EI_DATA]
        ELFDATA2LSB = 1,        /* 2's complement, little endian */
        ELFDATA2MSB = 2         /* 2's complement, big endian */
    };
    enum { // e_ident[EI_OSABI]
        ELFOSABI_NONE = 0,      // SYSV
        ELFOSABI_NETBSD = 2,
        ELFOSABI_LINUX = 3,
        ELFOSABI_FREEBSD = 9,
        ELFOSABI_OPENBSD = 12,
        ELFOSABI_ARM = 97
    };
    enum { // e_type
        ET_NONE = 0,            /* No file type */
        ET_REL  = 1,            /* Relocatable file */
        ET_EXEC = 2,            /* Executable file */
        ET_DYN  = 3,            /* Shared object file */
        ET_CORE = 4,            /* Core file */
    };
    enum { // e_machine
        EM_386    = 3,
        EM_PPC    = 20,
        EM_PPC64  = 21,
        EM_ARM    = 40,
        EM_X86_64 = 62,
    };
    enum { // e_version
        EV_CURRENT = 1,
    };
}
__attribute_packed;


// Program segment header.
struct BasePhdr
{
    enum { // p_type
        PT_LOAD    = 1,         /* Loadable program segment */
        PT_DYNAMIC = 2,         /* Dynamic linking information */
        PT_INTERP  = 3,         /* Name of program interpreter */
        PT_NOTE    = 4,         /* Auxiliary information (esp. OpenBSD) */
        PT_PHDR    = 6,         /* Entry for header table itself */
    };

    enum { // p_flags
        PF_X = 1,               /* Segment is executable */
        PF_W = 2,               /* Segment is writable */
        PF_R = 4,               /* Segment is readable */
    };
}
__attribute_packed;


struct BaseShdr
{
    enum SHT { // sh_type
        SHT_NULL = 0,           /* Section header table entry unused */
        SHT_PROGBITS = 1,       /* Program data */
        SHT_SYMTAB = 2,         /* Symbol table */
        SHT_STRTAB = 3,         /* String table */
        SHT_RELA = 4,           /* Relocation entries with addends */
        SHT_HASH = 5,           /* Symbol hash table */
        SHT_DYNAMIC = 6,        /* Dynamic linking information */
        SHT_NOTE = 7,           /* Notes */
        SHT_NOBITS = 8,         /* Program space with no data (bss) */
        SHT_REL = 9,            /* Relocation entries, no addends */
        SHT_SHLIB = 10,         /* Reserved */
        SHT_DYNSYM = 11,        /* Dynamic linker symbol table */
            /* 12, 13  hole */
        SHT_INIT_ARRAY = 14,    /* Array of constructors */
        SHT_FINI_ARRAY = 15,    /* Array of destructors */
        SHT_PREINIT_ARRAY = 16, /* Array of pre-constructors */
        SHT_GROUP = 17,         /* Section group */
        SHT_SYMTAB_SHNDX = 18,  /* Extended section indeces */
        SHT_GNU_LIBLIST = 0x6ffffff7,   /* Prelink library list */
    };

    enum SHF { // sh_flags
        SHF_WRITE      = (1 << 0),  /* Writable */
        SHF_ALLOC      = (1 << 1),  /* Occupies memory during execution */
        SHF_EXECINSTR  = (1 << 2),  /* Executable */
        SHF_MERGE      = (1 << 4),  /* Might be merged */
        SHF_STRINGS    = (1 << 5),  /* Contains nul-terminated strings */
        SHF_INFO_LINK  = (1 << 6),  /* 'sh_info' contains SHT index */
        SHF_LINK_ORDER = (1 << 7),  /* Preserve order after combining */
    };
}
__attribute_packed;


template <class TElfITypes>
struct Dyn
{
    typedef typename TElfITypes::Xword   Xword;
    typedef typename TElfITypes::Addr    Addr;

    Xword d_tag;
    Addr d_val;

    enum { // d_tag
        DT_NULL     =  0,       /* End flag */
        DT_NEEDED   =  1,       /* Name of needed library */
        DT_HASH     =  4,       /* Hash table of symbol names */
        DT_STRTAB   =  5,       /* String table */
        DT_SYMTAB   =  6,       /* Symbol table */
        DT_STRSZ    = 10,       /* Sizeof string table */
    };
}
__attribute_packed;


struct BaseSym
{
    enum { // st_bind (high 4 bits of st_info)
        STB_LOCAL   =   0,      /* Local symbol */
        STB_GLOBAL  =   1,      /* Global symbol */
        STB_WEAK    =   2,      /* Weak symbol */
    };

    enum { // st_type (low 4 bits of st_info)
        STT_NOTYPE  =   0,      /* Symbol type is unspecified */
        STT_OBJECT  =   1,      /* Symbol is a data object */
        STT_FUNC    =   2,      /* Symbol is a code object */
        STT_SECTION =   3,      /* Symbol associated with a section */
        STT_FILE    =   4,      /* Symbol's name is file name */
        STT_COMMON  =   5,      /* Symbol is a common data object */
        STT_TLS     =   6,      /* Symbol is thread-local data object*/
    };

    enum { // st_other (visibility)
        STV_DEFAULT  =  0,      /* Default symbol visibility rules */
        STV_INTERNAL =  1,      /* Processor specific hidden class */
        STV_HIDDEN   =  2,      /* Sym unavailable in other modules */
        STV_PROTECTED=  3,      /* Not preemptible, not exported */
    };

    enum { // st_shndx
        SHN_UNDEF   =   0,      /* Undefined section */
        SHN_ABS     =   0xfff1, /* Associated symbol is absolute */
        SHN_COMMON  =   0xfff2, /* Associated symbol is common */
    };
}
__attribute_packed;


} // namespace N_Elf


/*************************************************************************
// N_Elf32
**************************************************************************/

namespace N_Elf32 {

template <class TElfITypes>
struct Phdr : public N_Elf::BasePhdr
{
    typedef typename TElfITypes::Word    Word;
    typedef typename TElfITypes::Addr    Addr;
    typedef typename TElfITypes::Off     Off;

    Word p_type;                /* Segment type */
    Off  p_offset;              /* Segment file offset */
    Addr p_vaddr;               /* Segment virtual address */
    Addr p_paddr;               /* Segment physical address */
    Word p_filesz;              /* Segment size in file */
    Word p_memsz;               /* Segment size in memory */
    Word p_flags;               /* Segment flags */
    Word p_align;               /* Segment alignment */
}
__attribute_packed;


template <class TElfITypes>
struct Shdr : public N_Elf::BaseShdr
struct Shdr
{
    typedef typename TElfITypes::Word    Word;
    typedef typename TElfITypes::Addr    Addr;
    typedef typename TElfITypes::Off     Off;

    Word sh_name;               /* Section name (string tbl index) */
    Word sh_type;               /* Section type */
    Word sh_flags;              /* Section flags */
    Addr sh_addr;               /* Section virtual addr at execution */
    Off  sh_offset;             /* Section file offset */
    Word sh_size;               /* Section size in bytes */
    Word sh_link;               /* Link to another section */
    Word sh_info;               /* Additional section information */
    Word sh_addralign;          /* Section alignment */
    Word sh_entsize;            /* Entry size if section holds table */
}
__attribute_packed;


template <class TElfITypes>
struct Sym : public N_Elf::BaseSym
{
    typedef typename TElfITypes::Word    Word;
    typedef typename TElfITypes::Addr    Addr;
    typedef typename TElfITypes::Section Section;

    Word st_name;               /* symbol name (index into string table) */
    Addr st_value;              /* symbol value */
    Word st_size;               /* symbol size */
    unsigned char st_info;      /* symbol type and binding */
    unsigned char st_other;     /* symbol visibility */
    Section st_shndx;           /* section index */

    static unsigned int  get_st_bind(unsigned x) { return 0xf & (x>>4); }
    static unsigned int  get_st_type(unsigned x) { return 0xf &  x    ; }
    static unsigned char get_st_info(unsigned bind, unsigned type) { return (bind<<4) + (0xf & type); }
}
__attribute_packed;


} // namespace N_Elf32


/*************************************************************************
// N_Elf64
**************************************************************************/

namespace N_Elf64 {

template <class TElfITypes>
struct Phdr : public N_Elf::BasePhdr
{
    typedef typename TElfITypes::Word    Word;
    typedef typename TElfITypes::Xword   Xword;
    typedef typename TElfITypes::Addr    Addr;
    typedef typename TElfITypes::Off     Off;

    Word  p_type;               /* Segment type */
    Word  p_flags;              /* Segment flags */
    Off   p_offset;             /* Segment file offset */
    Addr  p_vaddr;              /* Segment virtual address */
    Addr  p_paddr;              /* Segment physical address */
    Xword p_filesz;             /* Segment size in file */
    Xword p_memsz;              /* Segment size in memory */
    Xword p_align;              /* Segment alignment */
}
__attribute_packed;


template <class TElfITypes>
struct Shdr : public N_Elf::BaseShdr
{
    typedef typename TElfITypes::Word    Word;
    typedef typename TElfITypes::Xword   Xword;
    typedef typename TElfITypes::Addr    Addr;
    typedef typename TElfITypes::Off     Off;

    Word  sh_name;              /* Section name (string tbl index) */
    Word  sh_type;              /* Section type */
    Xword sh_flags;             /* Section flags */
    Addr  sh_addr;              /* Section virtual addr at execution */
    Off   sh_offset;            /* Section file offset */
    Xword sh_size;              /* Section size in bytes */
    Word  sh_link;              /* Link to another section */
    Word  sh_info;              /* Additional section information */
    Xword sh_addralign;         /* Section alignment */
    Xword sh_entsize;           /* Entry size if section holds table */
}
__attribute_packed;


template <class TElfITypes>
struct Sym
{
    typedef typename TElfITypes::Word    Word;
    typedef typename TElfITypes::Xword   Xword;
    typedef typename TElfITypes::Addr    Addr;
    typedef typename TElfITypes::Section Section;

    Word st_name;               /* symbol name (index into string table) */
    unsigned char st_info;      /* symbol type and binding */
    unsigned char st_other;     /* symbol visibility */
    Section st_shndx;           /* section index */
    Addr st_value;              /* symbol value */
    Xword st_size;              /* symbol size */
}
__attribute_packed;


} // namespace N_Elf64


/*************************************************************************
// aggregate types in an ElfClass
**************************************************************************/

namespace N_Elf {

template <class TP>
struct ElfClass_32
{
    typedef TP BeLePolicy;

    // integral types
    typedef typename TP::U16 U16;
    typedef typename TP::U32 U32;
    typedef typename TP::U64 U64;
    typedef N_Elf::ElfITypes<U16, U32, U32, U32, U32> ElfITypes;

    // ELF types
    typedef N_Elf  ::Ehdr<ElfITypes> Ehdr;
    typedef N_Elf32::Phdr<ElfITypes> Phdr;
    typedef N_Elf32::Shdr<ElfITypes> Shdr;
    typedef N_Elf  ::Dyn <ElfITypes> Dyn;
    typedef N_Elf32::Sym <ElfITypes> Sym;

    static void compileTimeAssertions() {
        BeLePolicy::compileTimeAssertions();
        COMPILE_TIME_ASSERT(sizeof(Ehdr) == 52)
        COMPILE_TIME_ASSERT(sizeof(Phdr) == 32)
        COMPILE_TIME_ASSERT(sizeof(Shdr) == 40)
        COMPILE_TIME_ASSERT(sizeof(Dyn)  ==  8)
        COMPILE_TIME_ASSERT(sizeof(Sym)  == 16)
    }
};


template <class TP>
struct ElfClass_64
{
    typedef TP BeLePolicy;

    // integral types
    typedef typename TP::U16 U16;
    typedef typename TP::U32 U32;
    typedef typename TP::U64 U64;
    typedef N_Elf::ElfITypes<U16, U32, U64, U64, U64> ElfITypes;

    // ELF types
    typedef N_Elf  ::Ehdr<ElfITypes> Ehdr;
    typedef N_Elf64::Phdr<ElfITypes> Phdr;
    typedef N_Elf64::Shdr<ElfITypes> Shdr;
    typedef N_Elf  ::Dyn <ElfITypes> Dyn;
    typedef N_Elf64::Sym <ElfITypes> Sym;

    static void compileTimeAssertions() {
        BeLePolicy::compileTimeAssertions();
        COMPILE_TIME_ASSERT(sizeof(Ehdr) == 64)
        COMPILE_TIME_ASSERT(sizeof(Phdr) == 56)
        COMPILE_TIME_ASSERT(sizeof(Shdr) == 64)
        COMPILE_TIME_ASSERT(sizeof(Dyn)  == 16)
        COMPILE_TIME_ASSERT(sizeof(Sym)  == 24)
    }
};

} // namespace N_Elf


typedef N_Elf::ElfClass_32<N_BELE_CTP::HostPolicy> ElfClass_Host32;
typedef N_Elf::ElfClass_64<N_BELE_CTP::HostPolicy> ElfClass_Host64;
typedef N_Elf::ElfClass_32<N_BELE_CTP::BEPolicy>   ElfClass_BE32;
typedef N_Elf::ElfClass_64<N_BELE_CTP::BEPolicy>   ElfClass_BE64;
typedef N_Elf::ElfClass_32<N_BELE_CTP::LEPolicy>   ElfClass_LE32;
typedef N_Elf::ElfClass_64<N_BELE_CTP::LEPolicy>   ElfClass_LE64;


/*************************************************************************
// typedef shortcuts
**************************************************************************/

typedef ElfClass_Host32::Ehdr Elf32_Ehdr;
typedef ElfClass_Host32::Phdr Elf32_Phdr;
typedef ElfClass_Host32::Shdr Elf32_Shdr;
typedef ElfClass_Host32::Dyn  Elf32_Dyn;
typedef ElfClass_Host32::Sym  Elf32_Sym;

typedef ElfClass_Host64::Ehdr Elf64_Ehdr;
typedef ElfClass_Host64::Phdr Elf64_Phdr;
typedef ElfClass_Host64::Shdr Elf64_Shdr;
typedef ElfClass_Host64::Dyn  Elf64_Dyn;
typedef ElfClass_Host64::Sym  Elf64_Sym;

typedef ElfClass_BE32::Ehdr   Elf_BE32_Ehdr;
typedef ElfClass_BE32::Phdr   Elf_BE32_Phdr;
typedef ElfClass_BE32::Shdr   Elf_BE32_Shdr;
typedef ElfClass_BE32::Dyn    Elf_BE32_Dyn;
typedef ElfClass_BE32::Sym    Elf_BE32_Sym;

typedef ElfClass_BE64::Ehdr   Elf_BE64_Ehdr;
typedef ElfClass_BE64::Phdr   Elf_BE64_Phdr;
typedef ElfClass_BE64::Shdr   Elf_BE64_Shdr;
typedef ElfClass_BE64::Dyn    Elf_BE64_Dyn;
typedef ElfClass_BE64::Sym    Elf_BE64_Sym;

typedef ElfClass_LE32::Ehdr   Elf_LE32_Ehdr;
typedef ElfClass_LE32::Phdr   Elf_LE32_Phdr;
typedef ElfClass_LE32::Shdr   Elf_LE32_Shdr;
typedef ElfClass_LE32::Dyn    Elf_LE32_Dyn;
typedef ElfClass_LE32::Sym    Elf_LE32_Sym;

typedef ElfClass_LE64::Ehdr   Elf_LE64_Ehdr;
typedef ElfClass_LE64::Phdr   Elf_LE64_Phdr;
typedef ElfClass_LE64::Shdr   Elf_LE64_Shdr;
typedef ElfClass_LE64::Dyn    Elf_LE64_Dyn;
typedef ElfClass_LE64::Sym    Elf_LE64_Sym;


#endif /* already included */


/*
vi:ts=4:et
*/


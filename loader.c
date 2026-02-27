#include "common.h"
#include "mips.h"
#include "memory.h"

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

/*******************************************
 * ELF LOADING - Loads the program details *
 *               for the emulated process. *
 *                                         *
 *               The program is in the elf *
 *               executable format. This   *
 *               function parses and crea- *
 *               tes segments for each co- *
 *               mponent of the program so *
 *               the emulator can address  *
 *               it.                       *
 *******************************************/

void loader_load_ehdr(FILE *elf, Elf32_Ehdr **ehdr) {
    assert((*ehdr = malloc(sizeof(Elf32_Ehdr))));
    assert(fread(*ehdr, 1, sizeof(Elf32_Ehdr), elf) 
        == sizeof(Elf32_Ehdr));
    assert((*ehdr)->e_machine == EM_MIPS &&
            "only support MIPS elf");
    assert((*ehdr)->e_ident[EI_CLASS] == ELFCLASS32 &&
            "only support 32-bit elf");
    assert((*ehdr)->e_ident[EI_DATA] == ELFDATA2LSB &&
            "only support little endian");
    assert((*ehdr)->e_type == ET_EXEC &&
            "only support executable elf");
}

void loader_load_phdr(FILE *elf, Elf32_Ehdr *ehdr, 
                      Elf32_Phdr **phdr) {
    size_t size_phdrs = 
        sizeof(Elf32_Phdr) * ehdr->e_phnum;
    assert((*phdr = malloc(size_phdrs)) &&
            "Couldn't allocate memory for program headers");
    assert(fseek(elf, ehdr->e_phoff, SEEK_SET) == 0 &&
            "Couldn't seek to program header start");
    assert(fread(*phdr, 1, size_phdrs, elf) == size_phdrs &&
            "Couldn't read program headers");
}

void loader_load_shdr(FILE *elf, Elf32_Ehdr *ehdr, 
                      Elf32_Shdr **shdr) {
    size_t size_shdrs = 
        sizeof(Elf32_Shdr) * ehdr->e_shnum;
    assert((*shdr = malloc(size_shdrs)) &&
            "Couldn't allocate memory for section headers");
    assert(fseek(elf, ehdr->e_shoff, SEEK_SET) == 0 &&
            "Couldn't seek to section header start");
    assert(fread(*shdr, 1, size_shdrs, elf) == size_shdrs &&
            "Couldn't read section headers");
}

void loader_load_symbols(FILE *elf, 
                         Elf32_Sym **symbols, Elf32_Shdr symtab_hdr, 
                         char      **strings, Elf32_Shdr strtab_hdr) {
    /* allocate symbols and strings */
    *symbols = malloc(symtab_hdr.sh_size);
    *strings = malloc(strtab_hdr.sh_size);

    assert(*symbols && *strings &&
        "Couldn't allocate memory for symbols or strings");

    /* read symbols */
    assert(fseek(elf, symtab_hdr.sh_offset, SEEK_SET) == 0 &&
            "Couldn't seek to symbol section start");
    assert(fread(*symbols, 1, symtab_hdr.sh_size, elf) == symtab_hdr.sh_size &&
            "Couldn't read symbol section");

    /* read strings */
    assert(fseek(elf, strtab_hdr.sh_offset, SEEK_SET) == 0 &&
            "Couldn't seek to string section start");
    assert(fread(*strings, 1, strtab_hdr.sh_size, elf) == strtab_hdr.sh_size &&
            "Couldn't read string section");
}

void loader_load_segments(struct memory *memory, FILE *elf, 
                          Elf32_Ehdr *ehdr, Elf32_Phdr *phdrs) {
    for (size_t h = 0; h < ehdr->e_phnum; h++) {
        /* if the header is PT_LOAD load into memory */
        if ((phdrs + h)->p_type != PT_LOAD) {
            continue;
        }

        /* define the heap start */
        if (((phdrs+h)->p_vaddr + (phdrs+h)->p_memsz) > memory->heap_start) {
            memory->heap_start = ALIGN(((phdrs+h)->p_vaddr + (phdrs+h)->p_memsz), 0x1000);
            memory->heap_end   = memory->heap_start;
        }
        
        /* create the load segment setting lower, upper, perm, and size */
        struct segment *segment = create_segment(memory, (phdrs+h)->p_vaddr, 
                                                         (phdrs+h)->p_vaddr + (phdrs+h)->p_memsz - 1,
                                                         (phdrs+h)->p_flags,
                                                         (phdrs+h)->p_memsz);
        
        /* seek to the start of the segment in the elf */
        assert(fseek(elf, (phdrs + h)->p_offset, SEEK_SET) == 0 &&
                "Couldn't seek to elf segment");

        /* read contents of elf into segment */
        assert((fread(segment->segment, 1, (phdrs+h)->p_filesz, elf) == (phdrs+h)->p_filesz) &&
                "Couldn't read all bytes from elf segment");

        /* if the file size is smaller than memory size then remaining is bss and needs to be cleared*/
        if ((phdrs+h)->p_memsz > (phdrs+h)->p_filesz) {
            memset(segment->segment + (phdrs+h)->p_filesz, 
                0, (phdrs+h)->p_memsz - (phdrs+h)->p_filesz);
        }

    }
}

void loader_load_pc(struct mips *mips, struct memory *memory, FILE *elf, 
                    Elf32_Ehdr *ehdr, Elf32_Phdr *phdr, Elf32_Shdr *shdr) {
    /* set the entry point */
    mips->r[MIPS_R_PC] = ehdr->e_entry;
}
void loader_load_gp(struct mips *mips, struct memory *memory, FILE *elf, 
                    Elf32_Ehdr *ehdr, Elf32_Phdr *phdr, Elf32_Shdr *shdr) {
    bool loaded_gp = false;
    for (u32 h = 0; !loaded_gp && h < ehdr->e_shnum; h++) {
        /* if the header is SYMTAB find __gnu_local_gp */
        if ((shdr+h)->sh_type != SHT_SYMTAB)
            continue;
        
        /* load the symbol table and the string table */
        Elf32_Shdr symtab_hdr = shdr[h];
        Elf32_Shdr strtab_hdr = shdr[symtab_hdr.sh_link];
        
        /* load the symbols and strings from the elf */
        char      *strings = NULL;
        Elf32_Sym *symbols = NULL;
        loader_load_symbols(elf, &symbols, symtab_hdr, 
            &strings, strtab_hdr);
        
        /* search for __gnu_local_gp symbol */
        for (u32 s = 0; s < symtab_hdr.sh_size / sizeof(Elf32_Sym); s++) {
            /* retrieve the symbol name string */
            const char *symbol_name = 
                &strings[symbols[s].st_name];
            
            /* check if it is __gnu_local_gp */
            if ((loaded_gp = !strcmp(symbol_name, "_gp"))) {
                /* assign the correct gp start value */
                mips->r[MIPS_R_GP] = 
                    symbols[s].st_value;
                break;
            }
        }
        
        /* free all resources */
        free(symbols);
        free(strings);
    }

    assert(loaded_gp &&
        "__gnu_local_gp not found!");
}
void loader_load_sp(struct mips *mips, struct memory *memory, FILE *elf, 
                    Elf32_Ehdr *ehdr, Elf32_Phdr *phdr, Elf32_Shdr *shdr) {

    /* allocate stack space */
    struct segment *stack = create_segment(memory, STACK_TOP - STACK_SIZE,
                                                   STACK_TOP,
                                                   STACK_PERM,
                                                   STACK_SIZE);

    /* clear the stack */
    memset(stack->segment, 0, stack->upper - stack->lower);

    /* set stack top*/
    mips->r[MIPS_R_SP] = STACK_TOP;

    /* write aux_v */
    struct { u32 type; u32 value; } auxv[] = {
        {3,  ehdr->e_entry},            // AT_PHDR (approx, usually needs load addr)
        {4,  ehdr->e_phentsize},       // AT_PHENT
        {5,  ehdr->e_phnum},           // AT_PHNUM
        {6,  4096},                    // AT_PAGESZ
        {0,  0}                        // AT_NULL (Terminator)
    };

    for (s32 e = sizeof(auxv)/sizeof(*auxv) - 1; e >= 0; e--) {
        memory_write(memory, 
            (mips->r[MIPS_R_SP] -= 4), auxv[e].value, 4);
        memory_write(memory, 
            (mips->r[MIPS_R_SP] -= 4), auxv[e].type, 4);
    }
    
    /* write envp */
    mips->r[MIPS_R_SP] -= 4;
    memory_write(memory, mips->r[MIPS_R_SP], 0, 4);

    /* write argv */
    mips->r[MIPS_R_SP] -= 4;
    memory_write(memory, mips->r[MIPS_R_SP], 0, 4);

    /* write argc */
    mips->r[MIPS_R_SP] -= 4;
    memory_write(memory, mips->r[MIPS_R_SP], 0, 4);
}

void loader_elf(struct mips   *mips, 
                struct memory *memory, 
                const char *elffile) {
    FILE *elf = NULL;
    Elf32_Ehdr *ehdr = NULL;
    Elf32_Phdr *phdr = NULL;
    Elf32_Shdr *shdr = NULL;

    assert(memory);
    assert(elffile);
    
    assert((elf = fopen(elffile, "rb")));
    
    loader_load_ehdr(
        elf, &ehdr);

    loader_load_phdr(
        elf, ehdr, &phdr);

    loader_load_shdr(
        elf, ehdr, &shdr);

    loader_load_segments(
        memory, elf, ehdr, phdr);

    loader_load_pc(
        mips, memory, elf, ehdr, phdr, shdr);

    loader_load_gp(
        mips, memory, elf, ehdr, phdr, shdr);

    loader_load_sp(
        mips, memory, elf, ehdr, phdr, shdr);
    
    fclose(elf);
    free(shdr);
    free(phdr);
    free(ehdr);
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <elf.h>

#include "mips.h"
#include "memory.h"

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
void loader_elf(struct mips   *mips, 
                struct memory *memory, 
                const char *elffile) {
    assert(memory);
    assert(elffile);
    
    FILE *elf = NULL;
    assert((elf = fopen(elffile, "rb")));

    Elf32_Ehdr ehdr = {0};
    assert(fread(&ehdr, 1, sizeof(ehdr), elf) == sizeof(ehdr));

    /* check architecture */
    assert(ehdr.e_ident[EI_CLASS] == ELFCLASS32 &&
            "only support 32-bit elf");

    /* check byte ordering */
    switch (ehdr.e_ident[EI_DATA]) {
    case ELFDATA2LSB:
        memory->read  = memory_le_read;
        memory->write = memory_le_write;
        break;
    case ELFDATA2MSB:
        memory->read  = memory_be_read;
        memory->write = memory_be_write;
        break;
    default:
        assert(0 && "Unknown endianness");
    }

    /* check elf type */
    assert(ehdr.e_type == ET_EXEC &&
            "only support executable elf");

    /* check machine type */
    assert(ehdr.e_machine == EM_MIPS &&
            "only support MIPS elf");

    /* set the entry point */
    mips->r[MIPS_R_PC] = ehdr.e_entry;
    /* set some stack size */
    mips->r[MIPS_R_SP] = 0x100;

    Elf32_Phdr *phdrs = NULL;
    size_t size_phdrs = 
        sizeof(Elf32_Phdr) * ehdr.e_phnum;

    assert((phdrs = malloc(size_phdrs)) &&
            "Couldn't allocate memory for program headers");
    assert(fseek(elf, ehdr.e_phoff, SEEK_SET) == 0 &&
            "Couldn't seek to program header start");
    assert(fread(phdrs, 1, size_phdrs, elf) == size_phdrs &&
            "Couldn't read program headers");

    /* construct the segments */
    for (size_t h = 0; h < ehdr.e_phnum; h++) {
        /* if the header is PT_LOAD load into memory */
        if ((phdrs + h)->p_type != PT_LOAD)
            continue;
        
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
    }

    fclose(elf);
    free(phdrs);
}

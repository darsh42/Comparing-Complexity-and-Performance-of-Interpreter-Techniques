/*****************************************************
 *                   interpreter                     *
 *****************************************************
 * One of the emulation techniques tested in this i- *
 * nvestigation                                      *
 *****************************************************/
/* Total Complexity:
 *     + Switch => Branch Delay,
 *                 Primary Opcode,
 *                 Secondary Opcode,
 *                 Branch Opcode
 *              = 4
 *     + Token Threading
 *              = 2
 *     = 6
 */
#include "common.h"
#include "log.h"
#include "mips.h"
#include "memory.h"
#include "loader.h"

/* contains templates to construct instructions */
#include "instructions.h"

/* instruction generation */
void interpreter_switch(struct mips *mips, struct memory *memory) {
    for (; !mips->halted; ) {

        /* read the next opcode */
        memory_read(memory, mips->r[MIPS_R_PC], 
                    &mips->r[MIPS_R_CIR], 4);

        /* the below switch statement might seem a slight bit confusing
         * essentially there are 3 categories of instructions, primary,
         * secondary and branch categories.
         *
         * The value of the primary category specifies if it is a secondary
         * or branch category instruction.
         *
         * As such cases 0x0 and 0x1 in primary category correspond to seconday
         * and branch categories respectively
         * */

#ifdef __DISASSEMBLE__
#define X(type, formatter, impl, value, name)  \
        case value: {                          \
            type                               \
            impl                               \
            formatter(#name)                   \
            break;                             \
        }
#else // __DISASSEMBLE__
#define X(type, formatter, impl, value, name)  \
        case value: {                          \
            type                               \
            impl                               \
            break;                             \
        }
#endif // __DISASSEMBLE__

        /* process opcode */
        switch((mips->r[MIPS_R_CIR] >> OP_SHIFT) & OP_MASK) {
        /*  PRIMARY INSTRUCTION BLOCK */
        __INSTRUCTIONS_PRIMARY
        default:
            fprintf(stderr, "instruction: %08x: 0x%08x\n", 
                    mips->r[MIPS_R_PC], mips->r[MIPS_R_CIR]);
            assert(0 && "Unknown primary instruction");

    
        /* SECONDARY INSTRUCTION BLOCK */
        case 0x0:
        switch((mips->r[MIPS_R_CIR] >> FN_SHIFT) & FN_MASK) {
        __INSTRUCTIONS_SECONDARY
        default:
            fprintf(stderr, "instruction: %08x: 0x%08x\n", 
                    mips->r[MIPS_R_PC], mips->r[MIPS_R_CIR]);
            assert(0 && "Unknown secondary instruction");
        }
        break;

        /*  BRANCH INSTRUCTION BLOCK */
        case 0x1:
        switch((mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK) {
        __INSTRUCTIONS_BRANCH
        default:
            fprintf(stderr, "instruction: %08x: 0x%08x\n", 
                    mips->r[MIPS_R_PC], mips->r[MIPS_R_CIR]);
            assert(0 && "Unknown branch instruction");
        }
        break;
        }
#undef X

        INCREMENT_PC;
    }
}

#ifndef __MACRO_EXPANSION__
#ifdef __SWITCH_MAIN__
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s mips.elf\n", *argv);
        return 1;
    }

    struct mips   mips   = {};
    struct memory memory = {};

    /* handle any explicit object creation */
    create_memory(&memory);
    
    /* load elf into memory */
    loader_elf(&mips, &memory, *++argv);

    /* process the interpreter loop */
    interpreter_switch(&mips, &memory);

    /* clean up */
    delete_memory(&memory);

    return mips.status;
}
#endif // __SWITCH_MAIN__
#endif // __MACRO_EXPANSION__

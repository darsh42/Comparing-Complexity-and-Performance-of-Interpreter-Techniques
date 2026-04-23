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

#include "benchmark.h"
#include "instructions.h"

/* instruction generation */
void interpreter_switch(struct mips *mips, struct memory *memory) {
    for (; !mips->halted; ) {
        u32 cir;

        /* read the next opcode */
        memory_read_u32(memory, mips->r[MIPS_R_PC], &cir);

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

#if    defined(__DISASSEMBLE__)
#define X(type, formatter, impl, value, name)                                           \
        case value: {                                                                   \
            type                                                                        \
            impl                                                                        \
            formatter(#name)                                                            \
            break;                                                                      \
        }
#elif  defined(__PROFILE__)
#define X(type, formatter, impl, value, name)                                           \
        case value: {                                                                   \
            PROFILE_ENTER_INSTRUCTION                                                   \
            type                                                                        \
            impl                                                                        \
            PROFILE_EXIT_INSTRUCTION                                                    \
            break;                                                                      \
        }
#else
#define X(type, formatter, impl, value, name)                                           \
        case value: {                                                                   \
            type                                                                        \
            impl                                                                        \
            break;                                                                      \
        }
#endif

        /* process opcode */
        switch((cir >> OP_SHIFT) & OP_MASK) {
        /*  PRIMARY INSTRUCTION BLOCK */
        __INSTRUCTIONS_PRIMARY
        default:
            fprintf(stderr, "instruction: %08x: 0x%08x\n", 
                    mips->r[MIPS_R_PC], cir);
            assert(0 && "Unknown primary instruction");

    
        /* SECONDARY INSTRUCTION BLOCK */
        case 0x0:
        switch((cir >> FN_SHIFT) & FN_MASK) {
        __INSTRUCTIONS_SECONDARY
        default:
            fprintf(stderr, "instruction: %08x: 0x%08x\n", 
                    mips->r[MIPS_R_PC], cir);
            assert(0 && "Unknown secondary instruction");
        }
        break;

        /*  BRANCH INSTRUCTION BLOCK */
        case 0x1:
        switch((cir >> RT_SHIFT) & RT_MASK) {
        __INSTRUCTIONS_BRANCH
        default:
            fprintf(stderr, "instruction: %08x: 0x%08x\n", 
                    mips->r[MIPS_R_PC], cir);
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

    memory_create(&memory);
    loader_elf(&mips, &memory, *++argv);
    
#ifdef    __PROFILE__
    PROFILE_ENTER_INTERPRETER
#endif // __PROFILE__
    interpreter_switch(&mips, &memory);
#ifdef    __PROFILE__
    PROFILE_EXIT_INTERPRETER
#endif // __PROFILE__

    memory_delete(&memory);

    return mips.status;
}
#endif // __SWITCH_MAIN__
#endif // __MACRO_EXPANSION__

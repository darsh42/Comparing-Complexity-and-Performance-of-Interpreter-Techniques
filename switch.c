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

#ifdef __DISASSEMBLE__
#define CASE(type, formatter, impl, value)  \
        case value: {                       \
            type                            \
            impl                            \
            continue;                       \
        }
#define CASE_HALT(type, formatter, impl, value) \
        case value: {                       \
            type                            \
            impl                            \
            return;                         \
        }
#else // __DISASSEMBLE__
#define CASE(type, formatter, impl, value)  \
        case value: {                       \
            type                            \
            impl                            \
            formatter                       \
            continue;                       \
        }
#define CASE_HALT(type, formatter, impl, value) \
        case value: {                       \
            type                            \
            impl                            \
            return;                         \
        }
#endif // __DISASSEMBLE__

/* instruction generation */
void interpreter_switch(struct mips *mips, struct memory *memory) {
    for (;;) {
        /* handle branch delays */
        switch (mips->branch_s) {
        case DELAY:     
            mips->branch_s = TRANSFER; 
            break;
        case TRANSFER:  
            mips->branch_s = UNUSED;
            mips->r[MIPS_R_PC] = mips->branch_v;
            break;
        default:
            break;
        }

        /* read the next opcode */
        memory_read(memory, mips->r[MIPS_R_PC], 
                    &mips->r[MIPS_R_CIR], 4);

        /* increment program counter */
        mips->r[MIPS_R_PC] += 4;

        /* process opcode */
        switch((mips->r[MIPS_R_CIR] >> OP_SHIFT) & 
                                       OP_MASK) {
        case 0x0: goto secondary_op;
        case 0x1: goto    branch_op;
        CASE(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLEZ_IMPL, BLEZ_OP)
        CASE(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGTZ_IMPL, BGTZ_OP)
        CASE(ITP_TYPE_J_JAL, ITP_FORMAT_J_JAL, ITP_J_IMPL, J_OP)
        CASE(ITP_TYPE_J_JAL, ITP_FORMAT_J_JAL, ITP_JAL_IMPL, JAL_OP)
        CASE(ITP_TYPE_BRANCH_EQ_NE, ITP_FORMAT_BRANCH_EQ_NE, ITP_BEQ_IMPL, BEQ_OP)
        CASE(ITP_TYPE_BRANCH_EQ_NE, ITP_FORMAT_BRANCH_EQ_NE, ITP_BNE_IMPL, BNE_OP)
        CASE(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ADDI_IMPL,  ADDI_OP)
        CASE(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ADDIU_IMPL, ADDIU_OP)
        CASE(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_SLTI_IMPL,  SLTI_OP)
        CASE(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_SLTIU_IMPL, SLTIU_OP)
        CASE(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ANDI_IMPL,  ANDI_OP)
        CASE(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ORI_IMPL,   ORI_OP)
        CASE(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_XORI_IMPL,  XORI_OP)
        CASE(ITP_TYPE_LUI_IMM, ITP_FORMAT_LUI_IMM, ITP_LUI_IMPL, LUI_OP)
        CASE(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LB_IMPL,  LB_OP)
        CASE(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LH_IMPL,  LH_OP)
        CASE(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LWL_IMPL, LWL_OP)
        CASE(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LW_IMPL,  LW_OP)
        CASE(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LBU_IMPL, LBU_OP)
        CASE(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LHU_IMPL, LHU_OP)
        CASE(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LWR_IMPL, LWR_OP)
        CASE(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SB_IMPL,  SB_OP)
        CASE(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SH_IMPL,  SH_OP)
        CASE(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SWL_IMPL, SWL_OP)
        CASE(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SW_IMPL,  SW_OP)
        CASE(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SWR_IMPL, SWR_OP)
        default:
            assert(0 && "Unknown primary instruction");
        }
secondary_op:
        switch((mips->r[MIPS_R_CIR] >> FN_SHIFT) & 
                                       FN_MASK) {
        CASE_HALT(ITP_TYPE_BRK, ITP_FORMAT_SYSCALL, ITP_BRK_IMPL, BRK_FN)
        CASE(ITP_TYPE_SHIFT_IMM, ITP_FORMAT_SHIFT_IMM, ITP_SLL_IMPL, SLL_FN)
        CASE(ITP_TYPE_SHIFT_IMM, ITP_FORMAT_SHIFT_IMM, ITP_SRL_IMPL, SRL_FN)
        CASE(ITP_TYPE_SHIFT_IMM, ITP_FORMAT_SHIFT_IMM, ITP_SRA_IMPL, SRA_FN)
        CASE(ITP_TYPE_SHIFT_REG, ITP_FORMAT_SHIFT_REG, ITP_SLLV_IMPL, SLLV_FN)
        CASE(ITP_TYPE_SHIFT_REG, ITP_FORMAT_SHIFT_REG, ITP_SRLV_IMPL, SRLV_FN)
        CASE(ITP_TYPE_SHIFT_REG, ITP_FORMAT_SHIFT_REG, ITP_SRAV_IMPL, SRAV_FN)
        CASE(ITP_TYPE_JR, ITP_FORMAT_JR, ITP_JR_IMPL, JR_FN)
        CASE(ITP_TYPE_JALR, ITP_FORMAT_JALR, ITP_JALR_IMPL, JALR_FN)
        CASE(ITP_TYPE_SYSCALL, ITP_FORMAT_SYSCALL, ITP_SYSCALL_IMPL, SYSCALL_FN)
        CASE(ITP_TYPE_MF, ITP_FORMAT_MF, ITP_MFHI_IMPL, MFHI_FN)
        CASE(ITP_TYPE_MF, ITP_FORMAT_MF, ITP_MFLO_IMPL, MFLO_FN)
        CASE(ITP_TYPE_MT, ITP_FORMAT_MT, ITP_MTHI_IMPL, MTHI_FN)
        CASE(ITP_TYPE_MT, ITP_FORMAT_MT, ITP_MTLO_IMPL, MTLO_FN)
        CASE(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_MULT_IMPL,  MULT_FN)
        CASE(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_MULTU_IMPL, MULTU_FN)
        CASE(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_DIV_IMPL,  DIV_FN)
        CASE(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_DIVU_IMPL, DIVU_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_ADD_IMPL,  ADD_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_ADDU_IMPL, ADDU_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SUB_IMPL,  SUB_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SUBU_IMPL, SUBU_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SLT_IMPL,  SLT_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SLTU_IMPL, SLTU_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_AND_IMPL,  AND_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_OR_IMPL,   OR_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_XOR_IMPL,  XOR_FN)
        CASE(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_NOR_IMPL,  NOR_FN)
        default:
            assert(0 && "Unknown secondary instruction");
        }
branch_op:
        switch((mips->r[MIPS_R_CIR] >> RT_SHIFT) & 
                                       RT_MASK) {
        CASE(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLTZ_IMPL, BLTZ_RT)
        CASE(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGEZ_IMPL, BGEZ_RT)
        CASE(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLTZAL_IMPL, BLTZAL_RT)
        CASE(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGEZAL_IMPL, BGEZAL_RT)
        default:
            assert(0 && "Unknown branch instruction");
        }
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
    for(;;) {
        interpreter_switch(&mips, &memory);
    }

    /* clean up */
    delete_memory(&memory);

    return 0;
}
#endif // __SWITCH_MAIN__
#endif // __MACRO_EXPANSION__

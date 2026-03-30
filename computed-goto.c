#include "common.h"
#include "log.h"
#include "mips.h"
#include "memory.h"
#include "loader.h"

#include "instructions.h"

#define DISPATCH                                                \
    if (mips->halted)                                           \
        return;                                                 \
                                                                \
    /* read the next opcode */                                  \
    memory_read(                                                \
        memory, mips->r[MIPS_R_PC],                             \
        &mips->r[MIPS_R_CIR], 4);                               \
                                                                \
    /* goto the next label */                                   \
    goto *primary[                                              \
        (mips->r[MIPS_R_CIR] >> OP_SHIFT) & OP_MASK]
                                                                 
void interpreter_computed_goto(struct mips *mips, struct memory *memory) {     
    /* define each label array */                                
    static const void *primary[] = {
        [0] = &&do_secondary,
        [1] = &&do_branch,

        [J_OP]       = &&do_j,       [JAL_OP]    = &&do_jal,
        [BEQ_OP]     = &&do_beq,     [BNE_OP]    = &&do_bne,
        [BLEZ_OP]    = &&do_blez,    [BGTZ_OP]   = &&do_bgtz,
        [ADDI_OP]    = &&do_addi,    [ADDIU_OP]  = &&do_addiu,
        [SLTI_OP]    = &&do_slti,    [SLTIU_OP]  = &&do_sltiu,
        [ANDI_OP]    = &&do_andi,    [ORI_OP]    = &&do_ori,
        [XORI_OP]    = &&do_xori,    [LUI_OP]    = &&do_lui,
        [LB_OP]      = &&do_lb,      [LH_OP]     = &&do_lh,
        [LWL_OP]     = &&do_lwl,     [LW_OP]     = &&do_lw,
        [LL_OP]      = &&do_ll,      [LBU_OP]     = &&do_lbu,
        [LHU_OP]    = &&do_lhu,      [LWR_OP]     = &&do_lwr,
        [SH_OP]      = &&do_sh,      [SWL_OP]    = &&do_swl,
        [SW_OP]      = &&do_sw,      [SWR_OP]    = &&do_swr,
        [SB_OP]     = &&do_sb,       [SC_OP]      = &&do_sc,
        [RDHWR_OP]   = &&do_rdhwr,
    };
    static const void *secondary[] = {
        [SLL_FN]     = &&do_sll,     [SRL_FN]    = &&do_srl,
        [SRA_FN]     = &&do_sra,     [SLLV_FN]   = &&do_sllv,
        [SRLV_FN]    = &&do_srlv,    [SRAV_FN]   = &&do_srav,
        [JR_FN]      = &&do_jr,      [JALR_FN]   = &&do_jalr,
        [SYSCALL_FN] = &&do_syscall, [BRK_FN]    = &&do_brk,
        [MFHI_FN]    = &&do_mfhi,    [MTHI_FN]   = &&do_mthi,
        [MFLO_FN]    = &&do_mflo,    [MTLO_FN]   = &&do_mtlo,
        [MULT_FN]    = &&do_mult,    [MULTU_FN]  = &&do_multu,
        [DIV_FN]     = &&do_div,     [DIVU_FN]   = &&do_divu,
        [ADD_FN]     = &&do_add,     [ADDU_FN]   = &&do_addu,
        [SUB_FN]     = &&do_sub,     [SUBU_FN]   = &&do_subu,
        [AND_FN]     = &&do_and,     [OR_FN]     = &&do_or,
        [XOR_FN]     = &&do_xor,     [NOR_FN]    = &&do_nor,
        [SLT_FN]     = &&do_slt,     [SLTU_FN]   = &&do_sltu,
        [SPECIAL_FN] = &&do_special,
    };
    static const void *branch[] = {
        [BLTZ_RT]    = &&do_bltz,    [BGEZ_RT]   = &&do_bgez,
        [BLTZAL_RT]  = &&do_bltzal,  [BGEZAL_RT] = &&do_bgezal,
    };

    /* start dispatch chain */
    DISPATCH;
    
do_secondary:
    goto *secondary[
        (mips->r[MIPS_R_CIR] >> FN_SHIFT) & FN_MASK];
do_branch:
    goto *branch[
        (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK];

#ifndef   __DISASSEMBLE__
#define X(type, formatter, impl, opcode, name)                  \
    do_ ## name:                                                \
    {                                                           \
        type                                                    \
        impl                                                    \
        INCREMENT_PC;                                           \
        DISPATCH;                                               \
    }
#else  // __DISASSEMBLE__
#define X(type, formatter, impl, opcode, name)                  \
    do_ ## name:                                                \
    {                                                           \
        type                                                    \
        impl                                                    \
        INCREMENT_PC;                                           \
        formatter(#name)                                        \
        DISPATCH;                                               \
    }
#endif // __DISASSEMBLE__

    __INSTRUCTIONS

#undef X

}

#ifndef __MACRO_EXPANSION__
#ifdef __COMPUTED_GOTO_MAIN__
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s mips.elf\n", *argv);
        return 1;
    }

    struct mips   mips   = {};
    struct memory memory = {};

    /* handle any explicit object creation */
    memory_create(&memory);
    
    /* load elf into memory */
    loader_elf(&mips, &memory, *++argv);

    /* interpret the loaded binary */
    interpreter_computed_goto(&mips, &memory);

    /* clean up */
    memory_delete(&memory);

    return mips.status;
}
#endif // __COMPUTED_GOTO_MAIN__
#endif // __MACRO_EXPANSION__

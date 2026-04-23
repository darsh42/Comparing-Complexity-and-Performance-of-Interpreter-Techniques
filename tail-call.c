#include "common.h"
#include "log.h"
#include "mips.h"
#include "memory.h"
#include "loader.h"

#include "benchmark.h"
#include "instructions.h"

// to forward declare handlers
#define X(prologue, formatter, implementation, value, name)                 \
    static void interpret_##name(u32 cir, struct mips   * restrict mips,    \
                                          struct memory * restrict memory);
__INSTRUCTIONS
#undef X

static void interpret_secondary(u32 cir, struct mips   * restrict mips,
                                         struct memory * restrict memory);

static void interpret_branch(u32 cir, struct mips   * restrict mips,
                                      struct memory * restrict memory);

typedef void (*handler)(u32, struct mips* restrict, struct memory* restrict);

static handler primary[] = {
    [0] = &interpret_secondary,
    [1] = &interpret_branch,

    [J_OP]       = &interpret_j,       [JAL_OP]    = interpret_jal,
    [BEQ_OP]     = &interpret_beq,     [BNE_OP]    = interpret_bne,
    [BLEZ_OP]    = &interpret_blez,    [BGTZ_OP]   = interpret_bgtz,
    [ADDI_OP]    = &interpret_addi,    [ADDIU_OP]  = interpret_addiu,
    [SLTI_OP]    = &interpret_slti,    [SLTIU_OP]  = interpret_sltiu,
    [ANDI_OP]    = &interpret_andi,    [ORI_OP]    = interpret_ori,
    [XORI_OP]    = &interpret_xori,    [LUI_OP]    = interpret_lui,
    [LB_OP]      = &interpret_lb,      [LH_OP]     = interpret_lh,
    [LWL_OP]     = &interpret_lwl,     [LW_OP]     = interpret_lw,
    [LL_OP]      = &interpret_ll,      [LBU_OP]    = interpret_lbu,
    [LHU_OP]     = &interpret_lhu,     [LWR_OP]    = interpret_lwr,
    [SH_OP]      = &interpret_sh,      [SWL_OP]    = interpret_swl,
    [SW_OP]      = &interpret_sw,      [SWR_OP]    = interpret_swr,
    [SB_OP]      = &interpret_sb,      [SC_OP]     = interpret_sc,
    [RDHWR_OP]   = &interpret_rdhwr,
};
static handler secondary[] = {
    [SLL_FN]     = &interpret_sll,     [SRL_FN]    = interpret_srl,
    [SRA_FN]     = &interpret_sra,     [SLLV_FN]   = interpret_sllv,
    [SRLV_FN]    = &interpret_srlv,    [SRAV_FN]   = interpret_srav,
    [JR_FN]      = &interpret_jr,      [JALR_FN]   = interpret_jalr,
    [SYSCALL_FN] = &interpret_syscall, [BRK_FN]    = interpret_brk,
    [MFHI_FN]    = &interpret_mfhi,    [MTHI_FN]   = interpret_mthi,
    [MFLO_FN]    = &interpret_mflo,    [MTLO_FN]   = interpret_mtlo,
    [MULT_FN]    = &interpret_mult,    [MULTU_FN]  = interpret_multu,
    [DIV_FN]     = &interpret_div,     [DIVU_FN]   = interpret_divu,
    [ADD_FN]     = &interpret_add,     [ADDU_FN]   = interpret_addu,
    [SUB_FN]     = &interpret_sub,     [SUBU_FN]   = interpret_subu,
    [AND_FN]     = &interpret_and,     [OR_FN]     = interpret_or,
    [XOR_FN]     = &interpret_xor,     [NOR_FN]    = interpret_nor,
    [SLT_FN]     = &interpret_slt,     [SLTU_FN]   = interpret_sltu,
    [SPECIAL_FN] = &interpret_special,
};
static handler branch[] = {
    [BLTZ_RT]    = &interpret_bltz,    [BGEZ_RT]   = interpret_bgez,
    [BLTZAL_RT]  = &interpret_bltzal,  [BGEZAL_RT] = interpret_bgezal,
};

#if    defined(__DISASSEMBLE__)
#define X(prologue, formatter, implementation, value, name)                  \
    static void interpret_##name(u32 cir, struct mips   * restrict mips,     \
                                          struct memory * restrict memory) { \
        if (mips->halted)                                                    \
            return;                                                          \
        prologue                                                             \
        formatter(#name);                                                    \
        implementation                                                       \
        INCREMENT_PC;                                                        \
        memory_read_u32(memory, mips->r[MIPS_R_PC], &cir);                   \
        return primary[(cir >> OP_SHIFT) & OP_MASK](cir, mips, memory);      \
    }
#elif  defined(__PROFILE__)
#define X(prologue, formatter, implementation, value, name)                  \
    static void interpret_##name(u32 cir, struct mips   * restrict mips,     \
                                          struct memory * restrict memory) { \
        if (mips->halted)                                                    \
            return;                                                          \
        PROFILE_ENTER_INSTRUCTION                               \
        prologue                                                             \
        implementation                                                       \
        PROFILE_EXIT_INSTRUCTION                                \
        INCREMENT_PC;                                                        \
        memory_read_u32(memory, mips->r[MIPS_R_PC], &cir);                   \
        return primary[(cir >> OP_SHIFT) & OP_MASK](cir, mips, memory);      \
    }
#else
#define X(prologue, formatter, implementation, value, name)                  \
    static void interpret_##name(u32 cir, struct mips   * restrict mips,     \
                                          struct memory * restrict memory) { \
        if (mips->halted)                                                    \
            return;                                                          \
        prologue                                                             \
        implementation                                                       \
        INCREMENT_PC;                                                        \
        memory_read_u32(memory, mips->r[MIPS_R_PC], &cir);                   \
        return primary[(cir >> OP_SHIFT) & OP_MASK](cir, mips, memory);      \
    }
#endif

__INSTRUCTIONS

#undef X

static void interpret_secondary(u32 cir, struct mips   * restrict mips,
                                         struct memory * restrict memory) {
    return secondary[(cir >> FN_SHIFT) & FN_MASK](cir, mips, memory);
}

static void interpret_branch(u32 cir, struct mips   * restrict mips,
                                      struct memory * restrict memory) {
    return    branch[(cir >> RT_SHIFT) & RT_MASK](cir, mips, memory);
}

void interpreter_tail_call(struct mips *mips, struct memory *memory) {     
#ifdef __PROFILE_INTERPRETER__
    CALLGRIND_START_INSTRUMENTATION;
#endif // __PROFILE_INTERPRETER

    // create cir variable
    u32 cir;
    
    // read first instruction
    memory_read_u32(memory, mips->r[MIPS_R_PC], &cir);

    // dispatch first opcode
    primary[(cir >> OP_SHIFT) & OP_MASK](cir, mips, memory);

#ifdef __PROFILE_INTERPRETER__
    CALLGRIND_STOP_INSTRUMENTATION;
#endif // __PROFILE_INTERPRETER
}

#ifndef __MACRO_EXPANSION__
#ifdef  __TAIL_CALL_MAIN__
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

#ifdef    __PROFILE__
    PROFILE_ENTER_INTERPRETER
#endif // __PROFILE__
    /* interpret the loaded binary */
    interpreter_tail_call(&mips, &memory);
#ifdef    __PROFILE__
    PROFILE_EXIT_INTERPRETER
#endif // __PROFILE__
    
    /* clean up */
    memory_delete(&memory);

    return mips.status;
}
#endif // __TAIL_CALL_MAIN__
#endif // __MACRO_EXPANSION__

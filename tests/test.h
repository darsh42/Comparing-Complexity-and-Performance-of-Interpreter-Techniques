#ifndef __TEST_H__
#define __TEST_H__

/* General test case anatomy 
 *
 * static TEST_FUNC(name_of_test) {
 *      ** Define local case values 
 *
 *      ** Define the sequence of instructions
 *      TEST_SEQUENCE(START_ADDRESS
 *          ASM_INSTR,
 *          ASM_INSTR,
 *          ASM_INSTR);
 *
 *      ** Set processor initial state
 *      mips.r[MIPS_R_AT] = value;
 *
 *      ** Execute program based on passed implementation
 *      TEST_EXECUTE;
 *
 *      ** Define test assertion
 *      TEST_ASSERT(check, name, expected_msg, recieved_msg);
 *
 *      ** Complete test case cleanup
 *      delete_memory(&memory);
 * }
 * */

#include "mips.h"
#include "memory.h"


/* instruction assemblers */
#define ASSEMBLE_ALU_REG(FN, RS, RT, RD)            \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (RT & RT_MASK) << RT_SHIFT) |            \
    ((u32) (RD & RD_MASK) << RD_SHIFT) |            \
    ((u32) (RS & RS_MASK) << RS_SHIFT)
#define ASSEMBLE_ALU_IMM(OP, RS, RT, IMM16)         \
    ((u32) (OP    &    OP_MASK) <<    OP_SHIFT) |   \
    ((u32) (RS    &    RS_MASK) <<    RS_SHIFT) |   \
    ((u32) (RT    &    RT_MASK) <<    RT_SHIFT) |   \
    ((u32) (IMM16 & IMM16_MASK) << IMM16_SHIFT)
#define ASSEMBLE_BRANCH_EQ_NE(OP, RS, RT, IMM16)    \
    ((u32) (OP    &    OP_MASK) <<    OP_SHIFT) |   \
    ((u32) (RS    &    RS_MASK) <<    RS_SHIFT) |   \
    ((u32) (RT    &    RT_MASK) <<    RT_SHIFT) |   \
    ((u32) (IMM16 & IMM16_MASK) << IMM16_SHIFT)
#define ASSEMBLE_BRANCH_GENERIC(RT, RS, IMM16)      \
    ((u32) (0x1   &    OP_MASK) <<    OP_SHIFT) |   \
    ((u32) (RS    &    RS_MASK) <<    RS_SHIFT) |   \
    ((u32) (RT    &    RT_MASK) <<    RT_SHIFT) |   \
    ((u32) (IMM16 & IMM16_MASK) << IMM16_SHIFT)
#define ASSEMBLE_BRANCH_BLEZ_BGTZ(OP, RS, IMM16)    \
    ((u32) (OP    &    OP_MASK) <<    OP_SHIFT) |   \
    ((u32) (RS    &    RS_MASK) <<    RS_SHIFT) |   \
    ((u32) (IMM16 & IMM16_MASK) << IMM16_SHIFT)
#define ASSEMBLE_J_JAL(OP, IMM26)                   \
    ((u32) (OP    &    OP_MASK) <<    OP_SHIFT) |   \
    ((u32) (IMM26 & IMM26_MASK) << IMM26_SHIFT)
#define ASSEMBLE_JALR(FN, RS, RD)                   \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (RD & RD_MASK) << RD_SHIFT) |            \
    ((u32) (RS & RS_MASK) << RS_SHIFT)
#define ASSEMBLE_JR(FN, RS)                         \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (RS & RS_MASK) << RS_SHIFT)
#define ASSEMBLE_LOAD_IMM(OP, RS, RT, IMM16)        \
    ((u32) (OP & OP_MASK) << OP_SHIFT) |            \
    ((u32) (RS & RS_MASK) << RS_SHIFT) |            \
    ((u32) (RT & RT_MASK) << RT_SHIFT) |            \
    ((u32) (IMM16 & IMM16_MASK) << IMM16_SHIFT)
#define ASSEMBLE_LUI_IMM(OP, RT, IMM16)             \
    ((u32) (OP    &    OP_MASK) <<    OP_SHIFT) |   \
    ((u32) (RT    &    RT_MASK) <<    RT_SHIFT) |   \
    ((u32) (IMM16 & IMM16_MASK) << IMM16_SHIFT)
#define ASSEMBLE_MF(FN, RD)                         \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (RD & RD_MASK) << RD_SHIFT)
#define ASSEMBLE_MT(FN, RS)                         \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (RS & RS_MASK) << RS_SHIFT)
#define ASSEMBLE_MULT_DIV(OP, RS, RT)               \
    ((u32) (OP & OP_MASK) << OP_SHIFT) |            \
    ((u32) (RT & RT_MASK) << RT_SHIFT) |            \
    ((u32) (RS & RS_MASK) << RS_SHIFT)
#define ASSEMBLE_SHIFT_IMM(FN, RD, RT, SH)          \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (RT & RT_MASK) << RT_SHIFT) |            \
    ((u32) (RD & RD_MASK) << RD_SHIFT) |            \
    ((u32) (SH & SH_MASK) << SH_SHIFT)
#define ASSEMBLE_SHIFT_REG(FN, RD, RT, RS)          \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (RT & RT_MASK) << RT_SHIFT) |            \
    ((u32) (RD & RD_MASK) << RD_SHIFT) |            \
    ((u32) (RS & RS_MASK) << RS_SHIFT)
#define ASSEMBLE_STORE_IMM(OP, RT, RS, IMM16)       \
    ((u32) (OP    &    OP_MASK) <<    OP_SHIFT) |   \
    ((u32) (RS    &    RS_MASK) <<    RS_SHIFT) |   \
    ((u32) (RT    &    RT_MASK) <<    RT_SHIFT) |   \
    ((u32) (IMM16 & IMM16_MASK) << IMM16_SHIFT)
#define ASSEMBLE_SYSCALL(FN, IMM20)                 \
    ((u32) (FN & FN_MASK) << FN_SHIFT) |            \
    ((u32) (IMM20 & IMM20_MASK) << IMM20_SHIFT)
#define ASSEMBLE_NOP  0x00000000
#define ASSEMBLE_HALT 0xffffffff

/* test function delarator */
#define TEST_FUNC(name) \
    void name(void (*execute)(struct mips *, struct memory *))

#define TEST_START 0xbf000000
/* test sequence generator, loads assembled *
 * mneumonics (VAARGS), and set pc to START */
#define TEST_SEQUENCE(...)          \
    struct mips   mips   = {};      \
    struct memory memory = {};      \
    const u32 sequence[] = {        \
        __VA_ARGS__                 \
    };                              \
    tload(&memory, TEST_START,      \
          sizeof(sequence),         \
          sequence);                \
    mips.r[MIPS_R_PC] = TEST_START

#define TEST_EXECUTE \
    execute(&mips, &memory)

/* test assertion macro */
#define TEST_ASSERT(COND, NAME, EXPECTED, RECEIVED, ...) \
    tassert(COND, NAME, "\texpected: " EXPECTED "\n\treceived: " RECEIVED "\n", __VA_ARGS__)

void tload(struct memory *memory, u32 address, size_t count, const u32 *instructions);
void tassert(bool condition, const char *name, const char *diag, ...);

#include "test_alu_imm.h"
#include "test_alu_reg.h"
#include "test_branch_eq_ne.h"
#include "test_branch_generic.h"
#include "test_j_jal.h"
#include "test_jalr.h"
#include "test_jr.h"
#include "test_load_imm.h"
#include "test_lui_imm.h"
#include "test_mf.h"
#include "test_mt.h"
#include "test_mult_div.h"
#include "test_shift_imm.h"
#include "test_shift_reg.h"
#include "test_store_imm.h"
#include "test_syscalls.h"

/* tests case function signature */
typedef void (*tcase)(void (*)(struct mips *, struct memory *));

/* defined tests */
static const tcase interpreter_tests[] = {
    /* alu imm */
    &test_addi_positive,
    &test_addi_negetive,
    &test_addi_overflow,
    &test_addiu_positive,
    &test_addiu_negetive,
    &test_addiu_overflow,
    &test_slti_less,
    &test_slti_more,
    &test_slti_equal,
    &test_sltiu_less,
    &test_sltiu_more,
    &test_sltiu_equal,
    &test_sltiu_bit_extension_less,
    &test_sltiu_bit_extension_more,
    &test_sltiu_bit_extension_equal,
    &test_andi,
    &test_ori,
    &test_xori,

    /* alu reg */
    &test_add,
    &test_add_overflow,
    &test_addu,
    &test_addu_overflow,
    &test_sub,
    &test_sub_overflow,
    &test_subu_a,
    &test_subu_b,
    &test_and,
    &test_or,
    &test_xor,
    &test_nor,
    &test_slt_less,
    &test_slt_more,
    &test_slt_equal,
    &test_slt_negetive,
    &test_sltu_less,
    &test_sltu_more,
    &test_sltu_equal,
    &test_sltu_negetive,

    /* branch eq ne */
    &test_beq_before_delay,
    &test_beq_after_delay,
    &test_beq_jump_min,
    &test_beq_jump_max,
    &test_bne_before_delay,
    &test_bne_after_delay,
    &test_bne_jump_min,
    &test_bne_jump_max,

    /* branch generic */
    &test_bltz_compare_less,
    &test_bltz_compare_equal,
    &test_bltz_compare_more,
    &test_bgez_compare_less,
    &test_bgez_compare_equal,
    &test_bgez_compare_more,
    &test_blez_compare_less,
    &test_blez_compare_equal,
    &test_blez_compare_more,
    &test_bgtz_compare_less,
    &test_bgtz_compare_equal,
    &test_bgtz_compare_more,
    &test_bltzal_compare_less,
    &test_bltzal_compare_equal,
    &test_bltzal_compare_more,
    &test_bgezal_compare_less,
    &test_bgezal_compare_equal,
    &test_bgezal_compare_more,

    /* j jal */
    &test_j_address_max,
    &test_j_address_min,
    &test_jal,
    &test_jal_ra_modification,
    
    /* jalr */
    &test_jalr,
    &test_jalr_link_modification,
    
    /* jr */
    &test_jr_before_delay,
    &test_jr_after_delay,

    /* load immediate */
    &test_lb_before_delay,
    &test_lb_after_delay,
    &test_lh_before_delay,
    &test_lh_after_delay,
    &test_lw_before_delay,
    &test_lw_after_delay,
    &test_lbu_before_delay,
    &test_lbu_after_delay,
    &test_lhu_before_delay,
    &test_lhu_after_delay,

    /* lui */
    &test_lui_empty,
    &test_lui_filled,

    /* mf */
    &test_mfhi,
    &test_mflo,

    /* mt */
    &test_mthi,
    &test_mtlo,

    /* shift imm */
    &test_sll,
    &test_srl,
    &test_sra,

    /* shift reg */
    &test_sllv,
    &test_srlv,
    &test_srav,

    /* store immediate */
    &test_sb,
    &test_sh,
    &test_sw,

    /* syscalls */
    &test_syscalls_filehandling,
};

static const tcase jitcompiler_tests[] = {
};

#endif // __TEST_H__

#ifndef __MIPS_H__
#define __MIPS_H__

#include "syscall.h.in"

#define __INSTRUCTIONS_PRIMARY                                                                   \
    X(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLEZ_IMPL,    BLEZ_OP,    blez)    \
    X(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGTZ_IMPL,    BGTZ_OP,    bgtz)    \
    X(ITP_TYPE_J_JAL,          ITP_FORMAT_J_JAL,          ITP_J_IMPL,       J_OP,       j)       \
    X(ITP_TYPE_J_JAL,          ITP_FORMAT_J_JAL,          ITP_JAL_IMPL,     JAL_OP,     jal)     \
    X(ITP_TYPE_BRANCH_EQ_NE,   ITP_FORMAT_BRANCH_EQ_NE,   ITP_BEQ_IMPL,     BEQ_OP,     beq)     \
    X(ITP_TYPE_BRANCH_EQ_NE,   ITP_FORMAT_BRANCH_EQ_NE,   ITP_BNE_IMPL,     BNE_OP,     bne)     \
    X(ITP_TYPE_ALU_IMM,        ITP_FORMAT_ALU_IMM,        ITP_ADDI_IMPL,    ADDI_OP,    addi)    \
    X(ITP_TYPE_ALU_IMM,        ITP_FORMAT_ALU_IMM,        ITP_ADDIU_IMPL,   ADDIU_OP,   addiu)   \
    X(ITP_TYPE_ALU_IMM,        ITP_FORMAT_ALU_IMM,        ITP_SLTI_IMPL,    SLTI_OP,    slti)    \
    X(ITP_TYPE_ALU_IMM,        ITP_FORMAT_ALU_IMM,        ITP_SLTIU_IMPL,   SLTIU_OP,   sltiu)   \
    X(ITP_TYPE_ALU_IMM,        ITP_FORMAT_ALU_IMM,        ITP_ANDI_IMPL,    ANDI_OP,    andi)    \
    X(ITP_TYPE_ALU_IMM,        ITP_FORMAT_ALU_IMM,        ITP_ORI_IMPL,     ORI_OP,     ori)     \
    X(ITP_TYPE_ALU_IMM,        ITP_FORMAT_ALU_IMM,        ITP_XORI_IMPL,    XORI_OP,    xori)    \
    X(ITP_TYPE_LUI_IMM,        ITP_FORMAT_LUI_IMM,        ITP_LUI_IMPL,     LUI_OP,     lui)     \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LB_IMPL,      LB_OP,      lb)      \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LH_IMPL,      LH_OP,      lh)      \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LWL_IMPL,     LWL_OP,     lwl)     \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LW_IMPL,      LW_OP,      lw)      \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LBU_IMPL,     LBU_OP,     lbu)     \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LHU_IMPL,     LHU_OP,     lhu)     \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LWR_IMPL,     LWR_OP,     lwr)     \
    X(ITP_TYPE_LOAD_IMM,       ITP_FORMAT_LOAD_IMM,       ITP_LL_IMPL,      LL_OP,      ll)      \
    X(ITP_TYPE_STORE_IMM,      ITP_FORMAT_STORE_IMM,      ITP_SB_IMPL,      SB_OP,      sb)      \
    X(ITP_TYPE_STORE_IMM,      ITP_FORMAT_STORE_IMM,      ITP_SH_IMPL,      SH_OP,      sh)      \
    X(ITP_TYPE_STORE_IMM,      ITP_FORMAT_STORE_IMM,      ITP_SWL_IMPL,     SWL_OP,     swl)     \
    X(ITP_TYPE_STORE_IMM,      ITP_FORMAT_STORE_IMM,      ITP_SW_IMPL,      SW_OP,      sw)      \
    X(ITP_TYPE_STORE_IMM,      ITP_FORMAT_STORE_IMM,      ITP_SC_IMPL,      SC_OP,      sc)      \
    X(ITP_TYPE_STORE_IMM,      ITP_FORMAT_STORE_IMM,      ITP_SWR_IMPL,     SWR_OP,     swr)     \
    X(ITP_TYPE_RDHWR,          ITP_FORMAT_RDHWR,          ITP_RDHWR_IMPL,   RDHWR_OP,   rdhwr)

#define __INSTRUCTIONS_SECONDARY                                                                 \
    X(ITP_TYPE_BRK,            ITP_FORMAT_SYSCALL,        ITP_BRK_IMPL,     BRK_FN,     brk)     \
    X(ITP_TYPE_SHIFT_IMM,      ITP_FORMAT_SHIFT_IMM,      ITP_SLL_IMPL,     SLL_FN,     sll)     \
    X(ITP_TYPE_SHIFT_IMM,      ITP_FORMAT_SHIFT_IMM,      ITP_SRL_IMPL,     SRL_FN,     srl)     \
    X(ITP_TYPE_SHIFT_IMM,      ITP_FORMAT_SHIFT_IMM,      ITP_SRA_IMPL,     SRA_FN,     sra)     \
    X(ITP_TYPE_SHIFT_REG,      ITP_FORMAT_SHIFT_REG,      ITP_SLLV_IMPL,    SLLV_FN,    sllv)    \
    X(ITP_TYPE_SHIFT_REG,      ITP_FORMAT_SHIFT_REG,      ITP_SRLV_IMPL,    SRLV_FN,    srlv)    \
    X(ITP_TYPE_SHIFT_REG,      ITP_FORMAT_SHIFT_REG,      ITP_SRAV_IMPL,    SRAV_FN,    srav)    \
    X(ITP_TYPE_JR,             ITP_FORMAT_JR,             ITP_JR_IMPL,      JR_FN,      jr)      \
    X(ITP_TYPE_JALR,           ITP_FORMAT_JALR,           ITP_JALR_IMPL,    JALR_FN,    jalr)    \
    X(ITP_TYPE_SYSCALL,        ITP_FORMAT_SYSCALL,        ITP_SYSCALL_IMPL, SYSCALL_FN, syscall) \
    X(ITP_TYPE_SYSCALL,        ITP_FORMAT_SYSCALL,        ITP_SPECIAL_IMPL, SPECIAL_FN, special) \
    X(ITP_TYPE_MF,             ITP_FORMAT_MF,             ITP_MFHI_IMPL,    MFHI_FN,    mfhi)    \
    X(ITP_TYPE_MF,             ITP_FORMAT_MF,             ITP_MFLO_IMPL,    MFLO_FN,    mflo)    \
    X(ITP_TYPE_MT,             ITP_FORMAT_MT,             ITP_MTHI_IMPL,    MTHI_FN,    mthi)    \
    X(ITP_TYPE_MT,             ITP_FORMAT_MT,             ITP_MTLO_IMPL,    MTLO_FN,    mtlo)    \
    X(ITP_TYPE_MULT_DIV,       ITP_FORMAT_MULT_DIV,       ITP_MULT_IMPL,    MULT_FN,    mult)    \
    X(ITP_TYPE_MULT_DIV,       ITP_FORMAT_MULT_DIV,       ITP_MULTU_IMPL,   MULTU_FN,   multu)   \
    X(ITP_TYPE_MULT_DIV,       ITP_FORMAT_MULT_DIV,       ITP_DIV_IMPL,     DIV_FN,     div)     \
    X(ITP_TYPE_MULT_DIV,       ITP_FORMAT_MULT_DIV,       ITP_DIVU_IMPL,    DIVU_FN,    divu)    \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_ADD_IMPL,     ADD_FN,     add)     \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_ADDU_IMPL,    ADDU_FN,    addu)    \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_SUB_IMPL,     SUB_FN,     sub)     \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_SUBU_IMPL,    SUBU_FN,    subu)    \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_SLT_IMPL,     SLT_FN,     slt)     \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_SLTU_IMPL,    SLTU_FN,    sltu)    \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_AND_IMPL,     AND_FN,     and)     \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_OR_IMPL,      OR_FN,      or)      \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_XOR_IMPL,     XOR_FN,     xor)     \
    X(ITP_TYPE_ALU_REG,        ITP_FORMAT_ALU_REG,        ITP_NOR_IMPL,     NOR_FN,     nor)
#define __INSTRUCTIONS_BRANCH                                                                    \
    X(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLTZ_IMPL,    BLTZ_RT,    bltz)    \
    X(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGEZ_IMPL,    BGEZ_RT,    bgez)    \
    X(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLTZAL_IMPL,  BLTZAL_RT,  bltzal)  \
    X(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGEZAL_IMPL,  BGEZAL_RT,  bgezal)

#define __INSTRUCTIONS       \
    __INSTRUCTIONS_PRIMARY   \
    __INSTRUCTIONS_SECONDARY \
    __INSTRUCTIONS_BRANCH    \

#define __REGS           \
    X(MIPS_R_ZERO,     0, "%zr")\
    X(MIPS_R_AT,       1, "%at")\
    X(MIPS_R_V0,       2, "%v0")\
    X(MIPS_R_V1,       3, "%v1")\
    X(MIPS_R_A0,       4, "%a0")\
    X(MIPS_R_A1,       5, "%a1")\
    X(MIPS_R_A2,       6, "%a2")\
    X(MIPS_R_A3,       7, "%a3")\
    X(MIPS_R_T0,       8, "%t0")\
    X(MIPS_R_T1,       9, "%t1")\
    X(MIPS_R_T2,      10, "%t2")\
    X(MIPS_R_T3,      11, "%t3")\
    X(MIPS_R_T4,      12, "%t4")\
    X(MIPS_R_T5,      13, "%t5")\
    X(MIPS_R_T6,      14, "%t6")\
    X(MIPS_R_T7,      15, "%t7")\
    X(MIPS_R_S0,      16, "%s0")\
    X(MIPS_R_S1,      17, "%s1")\
    X(MIPS_R_S2,      18, "%s2")\
    X(MIPS_R_S3,      19, "%s3")\
    X(MIPS_R_S4,      20, "%s4")\
    X(MIPS_R_S5,      21, "%s5")\
    X(MIPS_R_S6,      22, "%s6")\
    X(MIPS_R_S7,      23, "%s7")\
    X(MIPS_R_T8,      24, "%t8")\
    X(MIPS_R_T9,      25, "%t9")\
    X(MIPS_R_K0,      26, "%k0")\
    X(MIPS_R_K1,      27, "%k1")\
    X(MIPS_R_GP,      28, "%gp")\
    X(MIPS_R_SP,      29, "%sp")\
    X(MIPS_R_FP,      30, "%fp")\
    X(MIPS_R_RA,      31, "%ra")\
    X(MIPS_R_HI,      32, "%hi")\
    X(MIPS_R_LO,      33, "%lo")\
    X(MIPS_R_PC,      34, "%pc")\
    X(MIPS_R_NPC,     35, "%npc")\
    X(MIPS_R_NNPC,    36, "%nnpc")\
    X(MIPS_R_CIR,     37, "%cir")\
    X(MIPS_R_GARBAGE, 38, "")\
    X(MIPS_R_REG_MAX, 39, "")\

#define INCREMENT_PC                                        \
    mips->r[MIPS_R_PC]   = mips->r[MIPS_R_NPC];             \
    mips->r[MIPS_R_NPC]  = mips->r[MIPS_R_NNPC];            \
    mips->r[MIPS_R_NNPC] = mips->r[MIPS_R_NNPC]+4;

#define __MIPS_SYSCALL_NR 4000
#define __SYSCALLS \
    X(MIPS_SYSCALL_READ,          3 + __MIPS_SYSCALL_NR, "syscall_read")  \
    X(MIPS_SYSCALL_WRITE,         4 + __MIPS_SYSCALL_NR, "syscall_write") \
    X(MIPS_SYSCALL_OPEN,          5 + __MIPS_SYSCALL_NR, "syscall_open")  \
    X(MIPS_SYSCALL_CLOSE,         6 + __MIPS_SYSCALL_NR, "syscall_close") \
    X(MIPS_SYSCALL_LSEEK,        19 + __MIPS_SYSCALL_NR, "syscall_lseek") \
    X(MIPS_SYSCALL_EXIT_GROUP,  210 + __MIPS_SYSCALL_NR, "syscall_exit_group") \
    X(MIPS_SYSCALL_TID_ADDR,    283 + __MIPS_SYSCALL_NR, "syscall_tid_addr")

enum MIPS_SECTION {
    OLD_BLOCK = 0,
    NEW_BLOCK = 1
};

enum MIPS_REG {
#define X(e, v, _) e = v,
    __REGS
#undef  X
};

enum MIPS_SYSCALL {
#define X(e, v, _) e = v,
    __SYSCALLS
#undef  X
};

enum branch_delay { 
     UNUSED, TRANSFER, DELAY
};

struct mips {
    u32 r[MIPS_R_REG_MAX];

#ifdef LOAD_DELAY_ENABLE
    u32 load_d, load_v;
#endif

    u32 status;
    u32 tidptr;
    u32 user_local_ptr;

    bool branched, halted;
};

static const char *register_names[] = {
#define X(n, _, s) [n] = s,
    __REGS
#undef  X
};

static const char *syscall_names[] = {
#define X(n, _, s) [n] = s,
    __SYSCALLS
#undef  X
};

static void print_mips_registers(struct mips *mips) {
    printf("mips-register contents\n");
    for (u32 r = 0; r < MIPS_R_REG_MAX; r++) {
        printf("%s: 0x%08x\n", register_names[r], mips->r[r]);
    }
}

#endif // __MIPS_H__

#ifndef __MIPS_H__
#define __MIPS_H__

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define CACHELINE_SIZE 64

#define U32MAX  0xffffffff
#define S32MIN ((s64)-2147483648LL)

#define FN_SHIFT 0
#define FN_MASK  0x3f
#define SH_SHIFT 6
#define SH_MASK  0x1f
#define RD_SHIFT 11
#define RD_MASK  0x1f
#define RT_SHIFT 16
#define RT_MASK  0x1f
#define RS_SHIFT 21
#define RS_MASK  0x1f
#define OP_SHIFT 26
#define OP_MASK  0x3f
#define IMM16_SHIFT 0
#define IMM16_MASK  0xffff
#define IMM20_SHIFT 0
#define IMM20_MASK  0xfffff
#define IMM26_SHIFT 0
#define IMM26_MASK  0x3ffffff

/* instruction op fields */
#define  ADD_FN 0x20
#define ADDU_FN 0x21
#define  SUB_FN 0x22
#define SUBU_FN 0x23
#define  AND_FN 0x24
#define   OR_FN 0x25
#define  XOR_FN 0x26
#define  NOR_FN 0x27
#define  SLT_FN 0x2A
#define SLTU_FN 0x2B

#define  ADDI_OP 0x08
#define ADDIU_OP 0x09
#define  SLTI_OP 0x0A
#define SLTIU_OP 0x0B
#define  ANDI_OP 0x0C
#define   ORI_OP 0x0D
#define  XORI_OP 0x0E

#define BEQ_OP 0x4
#define BNE_OP 0x5

#define   BLEZ_OP 0x06
#define   BGTZ_OP 0x07
#define   BLTZ_RT 0x00
#define   BGEZ_RT 0x01
#define BLTZAL_RT 0x10
#define BGEZAL_RT 0x11

#define   J_OP 0x2
#define JAL_OP 0x3

#define JALR_FN 0x9

#define JR_FN 0x8

#define    LB_OP 0x20
#define    LH_OP 0x21
#define   LWL_OP 0x22
#define    LW_OP 0x23
#define   LBU_OP 0x24
#define   LHU_OP 0x25
#define   LWR_OP 0x26

#define LUI_OP 0xF

#define MFHI_FN 0x10
#define MFLO_FN 0x12

#define MTHI_FN 0x11
#define MTLO_FN 0x13

#define MULT_FN 0x18
#define MULTU_FN 0x19

#define DIV_FN 0x1A
#define DIVU_FN 0x1B

#define SLL_FN 0x0
#define SRL_FN 0x2
#define SRA_FN 0x3

#define SLLV_FN 0x4
#define SRLV_FN 0x6
#define SRAV_FN 0x7

#define SB_OP 0x28
#define SH_OP 0x29
#define SWL_OP 0x2A
#define SW_OP 0x2B
#define SWR_OP 0x2E

#define SYSCALL_FN 0xC
#define BRK_FN 0xD

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
    X(MIPS_R_CIR,     35, "%cir")\
    X(MIPS_R_GARBAGE, 36, "")\
    X(MIPS_R_REG_MAX, 37, "")\

#define __SYSCALLS \
    X(MIPS_SYSCALL_READ,   0, "syscall_read")  \
    X(MIPS_SYSCALL_WRITE,  1, "syscall_write") \
    X(MIPS_SYSCALL_OPEN,   2, "syscall_open")  \
    X(MIPS_SYSCALL_CLOSE,  3, "syscall_close") \
    X(MIPS_SYSCALL_LSEEK,  4, "syscall_lseek") \
    X(MIPS_SYSCALL_MAX,    5, "syscall_max")

typedef  int64_t s64;
typedef  int32_t s32;
typedef  int16_t s16;
typedef   int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef  uint8_t  u8;

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

    u32 load_d, load_v, branch_v;
    enum branch_delay   branch_s;

    bool halted;
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

#endif // __MIPS_H__

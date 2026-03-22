#ifndef __MIPS_H__
#define __MIPS_H__

#include "syscall.h.in"

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

    u32 load_d, load_v, branch_v;
    enum branch_delay   branch_s;

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

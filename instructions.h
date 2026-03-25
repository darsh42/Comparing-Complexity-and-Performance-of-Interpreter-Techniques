#ifndef __INTERPRETER_TMPL_H__
#define __INTERPRETER_TMPL_H__

#include "syscalls.h"

/*****************************************************
 *               instruction builder                 *
 *****************************************************
 * when passed the type, implementation and name of  *
 * an instruction, the builder will generate a final *
 * implementation of the defined instruction.        *
 *                                                   *
 * The reasons for using templating style for creat- *
 * ing the instructions are:                         *
 *  - data retrieval can be abstracted to a set of   *
 *    instructions                                   *
 *  - load delays can be applied to all instructions *
 *  - instruction signatures are similar and can be  *
 *    mapped into an instruction table               *
 *  - easier to read the instruction implementations *
 *    as they are separated from the boiler plate    *
 *****************************************************/
#ifdef __LOAD_DELAY__
#define ITP_LOAD_DELAY                             \
        mips->r[mips->load_d]= mips->load_v;       \
        mips->r[MIPS_R_ZERO] = 0;                  \
        mips->load_d         = MIPS_R_GARBAGE;      
#else  // __LOAD_DELAY__
#define ITP_LOAD_DELAY
#endif // __LOAD_DELAY__

/*****************************************************
 *            instruction formatters                 *
 *****************************************************
 * Isolates the formatting mechanism of each of the  *
 * instruction types. This allows for easy changes to*
 * formats, disabling formatting during release, add-*
 * ding logic for runtime enabling of formatters, etc*
 *****************************************************/
#define ITP_FORMAT_NONE(name)
#define ITP_FORMAT_SHIFT_IMM(name)                                  \
    printf("  %x:\t%08x\t%s %s, %s, 0x%02x\n", mips->r[MIPS_R_PC],  \
                                               mips->r[MIPS_R_CIR], \
                                               name,                \
                                               register_names[rd],  \
                                               register_names[rt],  \
                                               sh);
#define ITP_FORMAT_SHIFT_REG(name)                              \
    printf("  %x:\t%08x\t%s %s, %s, %s\n", mips->r[MIPS_R_PC],  \
                                           mips->r[MIPS_R_CIR], \
                                           name,                \
                                           register_names[rd],  \
                                           register_names[rt],  \
                                           register_names[rs]);
#define ITP_FORMAT_JR(name)                             \
    printf("  %x:\t%08x\t%s %s\n", mips->r[MIPS_R_PC],  \
                                   mips->r[MIPS_R_CIR], \
                                   name,                \
                                   register_names[rs]);
#define ITP_FORMAT_JALR(name)              \
    printf("  %x:\t%08x\t%s %s, %s\n",  mips->r[MIPS_R_PC],  \
                                        mips->r[MIPS_R_CIR], \
                                        name,                \
                                        register_names[rd],  \
                                        register_names[rs]);
#define ITP_FORMAT_MF(name)                             \
    printf("  %x:\t%08x\t%s %s\n", mips->r[MIPS_R_PC],  \
                                   mips->r[MIPS_R_CIR], \
                                   name,                \
                                   register_names[rd]);
#define ITP_FORMAT_MT(name)                             \
    printf("  %x:\t%08x\t%s %s\n", mips->r[MIPS_R_PC],  \
                                   mips->r[MIPS_R_CIR], \
                                   name,                \
                                   register_names[rs]);
#define ITP_FORMAT_SYSCALL(name)                            \
    printf("  %x:\t%08x\t%s imm20\n",  mips->r[MIPS_R_PC],  \
                                       mips->r[MIPS_R_CIR], \
                                       name);

#define ITP_FORMAT_MULT_DIV(name)          \
    printf("  %x:\t%08x\t%s %s, %s\n",  mips->r[MIPS_R_PC],  \
                                        mips->r[MIPS_R_CIR], \
                                        name,                \
                                        register_names[rs],  \
                                        register_names[rt]);
#define ITP_FORMAT_ALU_REG(name)               \
    printf("  %x:\t%08x\t%s %s, %s, %s\n", mips->r[MIPS_R_PC],  \
                                           mips->r[MIPS_R_CIR], \
                                           name,                \
                                           register_names[rd],  \
                                           register_names[rt],  \
                                           register_names[rs]);
#define ITP_FORMAT_BRANCH_GENERIC(name)                               \
    printf("  %x:\t%08x\t%s %s, %hd (0x%08x)\n", mips->r[MIPS_R_PC],  \
                                                 mips->r[MIPS_R_CIR], \
                                                 name,                \
                                                 register_names[rs],  \
                                                 imm16,               \
                                                 mips->r[MIPS_R_NNPC]);
#define ITP_FORMAT_J_JAL(name)                              \
    printf("  %x:\t%08x\t%s 0x%07x (0x%08x)\n", mips->r[MIPS_R_PC],  \
                                                mips->r[MIPS_R_CIR], \
                                                name,                \
                                                imm26,               \
                                                mips->r[MIPS_R_NNPC]);
#define ITP_FORMAT_BRANCH_EQ_NE(name)           \
    printf("  %x:\t%08x\t%s %s, %s, %hd (0x%08x)\n", mips->r[MIPS_R_PC],  \
                                                     mips->r[MIPS_R_CIR], \
                                                     name,                \
                                                     register_names[rs],  \
                                                     register_names[rt],  \
                                                     imm16,               \
                                                     mips->r[MIPS_R_NNPC]);
#define ITP_FORMAT_ALU_IMM(name)            \
    printf("  %x:\t%08x\t%s %s, %s, %hd\n", mips->r[MIPS_R_PC],  \
                                            mips->r[MIPS_R_CIR], \
                                            name,                \
                                            register_names[rt],  \
                                            register_names[rs],  \
                                            imm16);
#define ITP_FORMAT_LUI_IMM(name)               \
    printf("  %x:\t%08x\t%s %s, 0x%04x\n", mips->r[MIPS_R_PC],  \
                                           mips->r[MIPS_R_CIR], \
                                           name,                \
                                           register_names[rt],  \
                                           imm16);
#define ITP_FORMAT_LOAD_IMM(name)                  \
    printf("  %x:\t%08x\t%s %s, 0x%04x[%s]\n", mips->r[MIPS_R_PC],  \
                                               mips->r[MIPS_R_CIR], \
                                               name,                \
                                               register_names[rt],  \
                                               imm16,               \
                                               register_names[rs]);
#define ITP_FORMAT_STORE_IMM(name)                 \
    printf("  %x:\t%08x\t%s %s, 0x%04x[%s]\n", mips->r[MIPS_R_PC],  \
                                               mips->r[MIPS_R_CIR], \
                                               name,                \
                                               register_names[rt],  \
                                               imm16,               \
                                               register_names[rs]);
#define ITP_FORMAT_RDHWR(name)                                      \
    printf("  %x:\t%08x\t%s %s, %d\n", mips->r[MIPS_R_PC],          \
                                       mips->r[MIPS_R_CIR],         \
                                       name,                        \
                                       register_names[rt],          \
                                       rd);

/*****************************************************
 *                 instruction types                 *
 *****************************************************
 * The type of the instruction determines the opcode *
 * fields required by an instruction                 *
 *                                                   *
 * Reasons for templating are as follows:            *
 * - data retieval is abstracted                     *
 * - hide boilerplate from implementation            *
 *****************************************************/
#define ITP_TYPE_NONE
#define ITP_TYPE_SHIFT_IMM                                         \
    u8  sh; u32 value_rt;                                          \
    enum MIPS_REG rt, rd;                                          \
    rt = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;              \
    rd = (mips->r[MIPS_R_CIR] >> RD_SHIFT) & RD_MASK;              \
    sh = (mips->r[MIPS_R_CIR] >> SH_SHIFT) & SH_MASK;              \
    value_rt = mips->r[rt];                                        \
    ITP_LOAD_DELAY
#define ITP_TYPE_SHIFT_REG                                         \
    u32 value_rs, value_rt;                                        \
    enum MIPS_REG rs, rt, rd;                                      \
    rs = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;              \
    rt = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;              \
    rd = (mips->r[MIPS_R_CIR] >> RD_SHIFT) & RD_MASK;              \
    value_rs = mips->r[rs]; value_rt = mips->r[rt];                \
    ITP_LOAD_DELAY
#define ITP_TYPE_JR                                                \
    u32 value_rs;                                                  \
    enum MIPS_REG rs;                                              \
    rs = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;              \
    value_rs = mips->r[rs];                                        \
    ITP_LOAD_DELAY
#define ITP_TYPE_JALR                                              \
    u32 value_rs;                                                  \
    enum MIPS_REG rs, rd;                                          \
    rs = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;              \
    rd = (mips->r[MIPS_R_CIR] >> RD_SHIFT) & RD_MASK;              \
    value_rs = mips->r[rs];                                        \
    ITP_LOAD_DELAY
#define ITP_TYPE_MF                                                \
    enum MIPS_REG rd;                                              \
    rd = (mips->r[MIPS_R_CIR] >> RD_SHIFT) & RD_MASK;              \
    ITP_LOAD_DELAY
#define ITP_TYPE_MT                                                \
    u32 value_rs;                                                  \
    enum MIPS_REG rs;                                              \
    rs = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;              \
    value_rs = mips->r[rs];                                        \
    ITP_LOAD_DELAY
#define ITP_TYPE_SYSCALL                                           \
    u32 imm20, v0, a0, a1, a2, a3;                                 \
    imm20 = (mips->r[MIPS_R_CIR] >> IMM20_SHIFT) & IMM20_MASK;     \
    v0 = mips->r[MIPS_R_V0];                                       \
    a0 = mips->r[MIPS_R_A0]; a1 = mips->r[MIPS_R_A1];              \
    a2 = mips->r[MIPS_R_A2]; a3 = mips->r[MIPS_R_A3];              \
    ITP_LOAD_DELAY
#define ITP_TYPE_BRK                                               \
    u32 imm20, v0, a0, a1, a2, a3;                                 \
    imm20 = (mips->r[MIPS_R_CIR] >> IMM20_SHIFT) & IMM20_MASK;     \
    v0 = mips->r[MIPS_R_V0];                                       \
    a0 = mips->r[MIPS_R_A0]; a1 = mips->r[MIPS_R_A1];              \
    a2 = mips->r[MIPS_R_A2]; a3 = mips->r[MIPS_R_A3];
#define ITP_TYPE_MULT_DIV                                          \
    u32 value_rs, value_rt;                                        \
    enum MIPS_REG rs, rt;                                          \
    rs = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;              \
    rt = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;              \
    value_rs = mips->r[rs]; value_rt = mips->r[rt];                \
    ITP_LOAD_DELAY
#define ITP_TYPE_ALU_REG                                           \
    u32 value_rs, value_rt;                                        \
    enum MIPS_REG rs, rt, rd;                                      \
    rs = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;              \
    rt = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;              \
    rd = (mips->r[MIPS_R_CIR] >> RD_SHIFT) & RD_MASK;              \
    value_rs = mips->r[rs]; value_rt = mips->r[rt];                \
    ITP_LOAD_DELAY
#define ITP_TYPE_BRANCH_GENERIC                                    \
    u32 value_rs;                                                  \
    enum MIPS_REG rs; u16 imm16;                                   \
    rs    = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;           \
    imm16 = (mips->r[MIPS_R_CIR] >> IMM16_SHIFT) & IMM16_MASK;     \
    value_rs = mips->r[rs];                                        \
    ITP_LOAD_DELAY
#define ITP_TYPE_J_JAL                                             \
    u32 imm26;                                                     \
    imm26 = (mips->r[MIPS_R_CIR] >> IMM26_SHIFT) & IMM26_MASK;     \
    ITP_LOAD_DELAY
#define ITP_TYPE_BRANCH_EQ_NE                                      \
    u16 imm16;                                                     \
    u32 value_rs, value_rt;                                        \
    enum MIPS_REG rs, rt;                                          \
    rs    = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;           \
    rt    = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;           \
    imm16 = (mips->r[MIPS_R_CIR] >> IMM16_SHIFT) & IMM16_MASK;     \
    value_rs = mips->r[rs]; value_rt = mips->r[rt];                \
    ITP_LOAD_DELAY
#define ITP_TYPE_ALU_IMM                                           \
    u16 imm16;                                                     \
    u32 value_rs;                                                  \
    enum MIPS_REG rs, rt;                                          \
    rs    = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;           \
    rt    = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;           \
    imm16 = (mips->r[MIPS_R_CIR] >> IMM16_SHIFT) & IMM16_MASK;     \
    value_rs = mips->r[rs];                                        \
    ITP_LOAD_DELAY
#define ITP_TYPE_LUI_IMM                                           \
    u16 imm16;                                                     \
    enum MIPS_REG rt;                                              \
    rt    = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;           \
    imm16 = (mips->r[MIPS_R_CIR] >> IMM16_SHIFT) & IMM16_MASK;     \
    ITP_LOAD_DELAY
#define ITP_TYPE_LOAD_IMM                                          \
    u16 imm16;                                                     \
    u32 value_rs;                                                  \
    enum MIPS_REG rs, rt;                                          \
    rs    = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;           \
    rt    = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;           \
    imm16 = (mips->r[MIPS_R_CIR] >> IMM16_SHIFT) & IMM16_MASK;     \
    value_rs = mips->r[rs];
#define ITP_TYPE_STORE_IMM                                         \
    u16 imm16;                                                     \
    u32 value_rs, value_rt;                                        \
    enum MIPS_REG rs, rt;                                          \
    rs    = (mips->r[MIPS_R_CIR] >> RS_SHIFT) & RS_MASK;           \
    rt    = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;           \
    imm16 = (mips->r[MIPS_R_CIR] >> IMM16_SHIFT) & IMM16_MASK;     \
    value_rs = mips->r[rs]; value_rt = mips->r[rt];                \
    ITP_LOAD_DELAY
#define ITP_TYPE_RDHWR                                             \
    enum MIPS_REG rt, rd;                                          \
    rt = (mips->r[MIPS_R_CIR] >> RT_SHIFT) & RT_MASK;              \
    rd = (mips->r[MIPS_R_CIR] >> RD_SHIFT) & RD_MASK;              \
    ITP_LOAD_DELAY


/* instruction implementations */
#define ITP_SLL_IMPL mips->r[rd] =             value_rt  << sh;
#define ITP_SRL_IMPL mips->r[rd] =             value_rt  >> sh;
#define ITP_SRA_IMPL mips->r[rd] = (u32)((s32) value_rt >> sh);

#define ITP_SLLV_IMPL mips->r[rd] =            value_rt  << (value_rs & 0x1f);
#define ITP_SRLV_IMPL mips->r[rd] =            value_rt  >> (value_rs & 0x1f);
#define ITP_SRAV_IMPL mips->r[rd] = (u32)((s32)value_rt >> (value_rs & 0x1f));

#define ITP_JR_IMPL                  \
    mips->branched       = true;     \
    mips->r[MIPS_R_NNPC] = value_rs;

#define ITP_JALR_IMPL                     \
    mips->branched = true;                \
    mips->r[rd] = mips->r[MIPS_R_PC] + 8; \
    mips->r[MIPS_R_NNPC] = value_rs;

#define ITP_SYSCALL_IMPL                                                                          \
    switch (v0) {                                                                                 \
    case __NR_exit:            mips_syscall_exit(mips, memory, a0, a1, a2, a3);            break; \
    case __NR_read:            mips_syscall_read (mips, memory, a0, a1, a2, a3);           break; \
    case __NR_write:           mips_syscall_write(mips, memory, a0, a1, a2, a3);           break; \
    case __NR_open:            mips_syscall_open (mips, memory, a0, a1, a2, a3);           break; \
    case __NR_close:           mips_syscall_close(mips, memory, a0, a1, a2, a3);           break; \
    case __NR_lseek:           mips_syscall_lseek(mips, memory, a0, a1, a2, a3);           break; \
    case __NR_ioctl:           mips_syscall_ioctl(mips, memory, a0, a1, a2, a3);           break; \
    case __NR_writev:          mips_syscall_writev(mips, memory, a0, a1, a2, a3);          break; \
    case __NR_exit_group:      mips_syscall_exit_group(mips, memory, a0, a1, a2, a3);      break; \
    case __NR_set_tid_address: mips_syscall_tid_addr(mips, memory, a0, a1, a2, a3);        break; \
    case __NR_set_thread_area: mips_syscall_set_thread_area(mips, memory, a0, a1, a2, a3); break; \
    default:                                                                                      \
        fprintf(stderr, "Unhandled Syscall: %d\n", v0);                                           \
        assert(0 && "unknown syscall");                                                           \
    }
#define ITP_BRK_IMPL     (void) imm20; \
    mips->halted = 1;

#define ITP_MFHI_IMPL mips->r[rd] = mips->r[MIPS_R_HI];
#define ITP_MFLO_IMPL mips->r[rd] = mips->r[MIPS_R_LO];

#define ITP_MTHI_IMPL mips->r[MIPS_R_HI] = value_rs;
#define ITP_MTLO_IMPL mips->r[MIPS_R_LO] = value_rs;

#define ITP_MULT_IMPL              \
    s64 a = (s64)(s32)value_rs;    \
    s64 b = (s64)(s32)value_rt;    \
    u64 r = a * b;                 \
    mips->r[MIPS_R_LO] = r >>  0;  \
    mips->r[MIPS_R_HI] = r >> 32;
#define ITP_MULTU_IMPL             \
    u64 a = (u64)(u32)value_rs;    \
    u64 b = (u64)(u32)value_rt;    \
    u64 r = a * b;                 \
    mips->r[MIPS_R_LO] = r >>  0;  \
    mips->r[MIPS_R_HI] = r >> 32;
#define ITP_DIV_IMPL                                      \
    if (value_rt == 0) {                                  \
        mips->r[MIPS_R_LO] = ((s32)value_rs > 0) ? -1: 1; \
        mips->r[MIPS_R_HI] = value_rs;                    \
    } else if ((s32) value_rt == -1 &&                    \
               (s32) value_rs == (s32) -0x80000000) {     \
        mips->r[MIPS_R_LO] = -0x80000000;                 \
        mips->r[MIPS_R_HI] = 0;                           \
    } else {                                              \
        mips->r[MIPS_R_LO] = value_rs/value_rt;           \
        mips->r[MIPS_R_HI] = value_rs%value_rt;           \
    }
#define ITP_DIVU_IMPL                          \
    if (value_rt) {                            \
        mips->r[MIPS_R_LO] = value_rs/value_rt;\
        mips->r[MIPS_R_HI] = value_rs%value_rt;\
    } else {                                   \
        mips->r[MIPS_R_LO] = 0xffffffff;       \
        mips->r[MIPS_R_HI] = value_rs;         \
    }

#define ITP_ADD_IMPL    \
    u64 a = value_rs;   \
    u64 b = value_rt;   \
    u64 r = a + b;      \
    if (r > U32MAX) {   \
        /* TRAP */      \
    } else {            \
        mips->r[rd] = r;\
    }
#define ITP_ADDU_IMPL mips->r[rd] = value_rs + value_rt;
#define ITP_SUB_IMPL    \
    s64 a = value_rs;   \
    s64 b = value_rt;   \
    s64 r = a - b;      \
    if (r < S32MIN) {   \
        /* TRAP */      \
    } else {            \
        mips->r[rd] = r;\
    }
#define ITP_SUBU_IMPL mips->r[rd] = value_rs - value_rt;
#define ITP_SLT_IMPL  mips->r[rd] = ((s32) value_rs) < ((s32) value_rt);
#define ITP_SLTU_IMPL mips->r[rd] = value_rs < value_rt;
#define ITP_AND_IMPL  mips->r[rd] = value_rs & value_rt;
#define ITP_OR_IMPL   mips->r[rd] = value_rs | value_rt;
#define ITP_XOR_IMPL  mips->r[rd] = value_rs ^ value_rt;
#define ITP_NOR_IMPL  mips->r[rd] = ~(value_rs | value_rt);

#define BRANCH                                                                \
    mips->branched       = true;                                              \
    mips->r[MIPS_R_NNPC] = mips->r[MIPS_R_PC] + (((s32)(s16)imm16) << 2) + 4;

#define ITP_BLTZ_IMPL   if ((s32) value_rs <  0) { BRANCH }
#define ITP_BGEZ_IMPL   if ((s32) value_rs >= 0) { BRANCH }
#define ITP_BLEZ_IMPL   if ((s32) value_rs <= 0) { BRANCH }
#define ITP_BGTZ_IMPL   if ((s32) value_rs >  0) { BRANCH }
#define ITP_BLTZAL_IMPL mips->r[MIPS_R_RA] = mips->r[MIPS_R_PC] + 8; if ((s32) value_rs <  0) { BRANCH }
#define ITP_BGEZAL_IMPL mips->r[MIPS_R_RA] = mips->r[MIPS_R_PC] + 8; if ((s32) value_rs >= 0) { BRANCH }

#define ITP_J_IMPL                                                          \
    mips->branched = true;                                                  \
    mips->r[MIPS_R_NNPC] = (mips->r[MIPS_R_PC] & 0xf0000000) | (imm26 << 2);

#define ITP_JAL_IMPL                                                        \
    mips->branched = true;                                                  \
    mips->r[MIPS_R_RA] = mips->r[MIPS_R_PC] + 8;                            \
    mips->r[MIPS_R_NNPC] = (mips->r[MIPS_R_PC] & 0xf0000000) | (imm26 << 2);

#define ITP_BEQ_IMPL if (value_rs == value_rt) { BRANCH }
#define ITP_BNE_IMPL if (value_rs != value_rt) { BRANCH }

#define ITP_ADDI_IMPL            \
    s64 a = value_rs;            \
    s64 b = (s64) (s16) imm16;   \
    s64 r = a + b;               \
    if (r > U32MAX) {            \
        /* TRAP */               \
    } else {                     \
        mips->r[rt] = r;         \
    }
#define ITP_ADDIU_IMPL mips->r[rt] = value_rs + (s32) (s16) imm16;
#define ITP_SLTI_IMPL  mips->r[rt] = ((s32) value_rs < (s32) (s16) imm16);
#define ITP_SLTIU_IMPL mips->r[rt] = value_rs < (u32) (s32) (s16) imm16;
#define ITP_ANDI_IMPL  mips->r[rt] = value_rs & imm16;
#define ITP_ORI_IMPL   mips->r[rt] = value_rs | imm16;
#define ITP_XORI_IMPL  mips->r[rt] = value_rs ^ imm16;

#define ITP_LUI_IMPL mips->r[rt] = imm16 << 16;

#define LOAD_COMPUTE_ADDRESS ((s32) (s16) imm16) + value_rs

#ifdef LOAD_DELAY_ENABLE
#define ITP_LB_IMPL                              \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 1);      \
    if (mips->load_d != rt) {                    \
        ITP_LOAD_DELAY                           \
    }                                            \
    mips->load_d = rt;                           \
    mips->load_v = (s32) (s8) read;
#define ITP_LH_IMPL                              \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 2);      \
    if (mips->load_d != rt) {                    \
        ITP_LOAD_DELAY                           \
    }                                            \
    mips->load_d = rt;                           \
    mips->load_v = (s32) (s16) read;
#define ITP_LW_IMPL                              \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 4);      \
    if (mips->load_d != rt) {                    \
        ITP_LOAD_DELAY                           \
    }                                            \
    mips->load_d = rt;                           \
    mips->load_v = read;
#define ITP_LBU_IMPL                             \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 1);      \
    if (mips->load_d != rt) {                    \
        ITP_LOAD_DELAY                           \
    }                                            \
    mips->load_d = rt;                           \
    mips->load_v = read;
#define ITP_LHU_IMPL                             \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 2);      \
    if (mips->load_d != rt) {                    \
        ITP_LOAD_DELAY                           \
    }                                            \
    mips->load_d = rt;                           \
    mips->load_v = read;
#define ITP_LWL_IMPL /* TODO: */                  \
    u32 read = 0;                                 \
    u32 address = LOAD_COMPUTE_ADDRESS;           \
    memory_read(memory, address & ~0x3, &read, 2);\
    if (mips->load_d != rt) {                     \
        ITP_LOAD_DELAY                            \
    }                                             \
    mips->load_v = read;
#define ITP_LWR_IMPL /* TODO: */                  \
    u32 read = 0;                                 \
    u32 address = LOAD_COMPUTE_ADDRESS;           \
    memory_read(memory, address & ~0x3, &read, 2);\
    if (mips->load_d != rt) {                     \
        ITP_LOAD_DELAY                            \
    }                                             \
    mips->load_v = read;
#else  // LOAD_DELAY_ENABLE
#define ITP_LB_IMPL                              \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 1);      \
    mips->r[rt] = (s32) (s8) read;
#define ITP_LH_IMPL                              \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 2);      \
    mips->r[rt] = (s32) (s16) read;
#define ITP_LW_IMPL                              \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 4);      \
    mips->r[rt] = read;
#define ITP_LBU_IMPL                             \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 1);      \
    mips->r[rt] = read;
#define ITP_LHU_IMPL                             \
    u32 read = 0;                                \
    u32 address = LOAD_COMPUTE_ADDRESS;          \
    memory_read(memory, address, &read, 2);      \
    mips->r[rt] = read;
#define ITP_LWL_IMPL /* TODO: */                  \
    u32 read = 0;                                 \
    u32 address = LOAD_COMPUTE_ADDRESS;           \
    memory_read(memory, address & ~0x3, &read, 2);\
    mips->r[rt] = read;
#define ITP_LWR_IMPL /* TODO: */                  \
    u32 read = 0;                                 \
    u32 address = LOAD_COMPUTE_ADDRESS;           \
    memory_read(memory, address & ~0x3, &read, 2);\
    mips->r[rt] = read;
#endif // LOAD_DELAY_ENABLE

#define STORE_COMPUTE_ADDRESS ((s32) (s16) imm16) + value_rs

#define ITP_SB_IMPL  memory_write(memory, STORE_COMPUTE_ADDRESS, value_rt, 1);
#define ITP_SH_IMPL  memory_write(memory, STORE_COMPUTE_ADDRESS, value_rt, 2);
#define ITP_SW_IMPL  memory_write(memory, STORE_COMPUTE_ADDRESS, value_rt, 4);

/* TODO: */
#define ITP_SWL_IMPL memory_write(memory, STORE_COMPUTE_ADDRESS, value_rt, 4);
#define ITP_SWR_IMPL memory_write(memory, STORE_COMPUTE_ADDRESS, value_rt, 4);

#define ITP_RDHWR_IMPL                             \
    switch (rd) {                                  \
    case 0: /* CPUNum */                           \
        mips->r[rt] = 0;                           \
        break;                                     \
    case 2: /* CC (Cycle Counter) */               \
        mips->r[rt] = 0;                           \
        break;                                     \
    case 3: /* CCRes (Cycle Counter Resolution) */ \
        mips->r[rt] = 1;                           \
        break;                                     \
    case 29: /* UserLocal (TLS Pointer) */         \
        mips->r[rt] = mips->user_local_ptr;        \
        break;                                     \
    default:                                       \
        mips->r[rt] = 0;                           \
        break;                                     \
    }

// only really used in a theaded interpreter

#endif // __INTERPRETER_TMPL_H__

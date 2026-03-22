#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef __MACRO_EXPANSION__
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "uthash.h"
#endif // __MACRO_EXPANSION__

#define CACHELINE_SIZE 64

#define U32MAX  0xffffffff
#define S32MIN ((s64)-2147483648LL)

typedef  int64_t s64;
typedef  int32_t s32;
typedef  int16_t s16;
typedef   int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef  uint8_t  u8;

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

#define RDHWR_OP 0x1f

#endif // __COMMON_H__

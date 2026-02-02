/*****************************************************
 *                   interpreter                     *
 *****************************************************
 * One of the emulation techniques tested in this i- *
 * nvestigation                                      *
 *****************************************************/
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "log.h"
#include "mips.h"
#include "memory.h"
#include "loader.h"

/* contains templates to construct instructions */
#include "interpreter_templates.h"

/* instruction generation */
ITP_INSN(ITP_TYPE_SHIFT_IMM, ITP_FORMAT_SHIFT_IMM, ITP_SLL_IMPL, sll)
ITP_INSN(ITP_TYPE_SHIFT_IMM, ITP_FORMAT_SHIFT_IMM, ITP_SRL_IMPL, srl)
ITP_INSN(ITP_TYPE_SHIFT_IMM, ITP_FORMAT_SHIFT_IMM, ITP_SRA_IMPL, sra)

ITP_INSN(ITP_TYPE_SHIFT_REG, ITP_FORMAT_SHIFT_REG, ITP_SLLV_IMPL, sllv)
ITP_INSN(ITP_TYPE_SHIFT_REG, ITP_FORMAT_SHIFT_REG, ITP_SRLV_IMPL, srlv)
ITP_INSN(ITP_TYPE_SHIFT_REG, ITP_FORMAT_SHIFT_REG, ITP_SRAV_IMPL, srav)

ITP_INSN(ITP_TYPE_JR, ITP_FORMAT_JR, ITP_JR_IMPL, jr)

ITP_INSN(ITP_TYPE_JALR, ITP_FORMAT_JALR, ITP_JALR_IMPL, jalr)

ITP_INSN(ITP_TYPE_SYSCALL, ITP_FORMAT_SYSCALL, ITP_SYSCALL_IMPL, syscall)
ITP_INSN(ITP_TYPE_SYSCALL, ITP_FORMAT_SYSCALL, ITP_BRK_IMPL, brk)

ITP_INSN(ITP_TYPE_MF, ITP_FORMAT_MF, ITP_MFHI_IMPL, mfhi)
ITP_INSN(ITP_TYPE_MF, ITP_FORMAT_MF, ITP_MFLO_IMPL, mflo)

ITP_INSN(ITP_TYPE_MT, ITP_FORMAT_MT, ITP_MTHI_IMPL, mthi)
ITP_INSN(ITP_TYPE_MT, ITP_FORMAT_MT, ITP_MTLO_IMPL, mtlo)

ITP_INSN(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_MULT_IMPL,  mult)
ITP_INSN(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_MULTU_IMPL, multu)
ITP_INSN(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_DIV_IMPL,  div)
ITP_INSN(ITP_TYPE_MULT_DIV, ITP_FORMAT_MULT_DIV, ITP_DIVU_IMPL, divu)

ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_ADD_IMPL,  add)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_ADDU_IMPL, addu)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SUB_IMPL,  sub)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SUBU_IMPL, subu)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SLT_IMPL,  slt)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_SLTU_IMPL, sltu)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_AND_IMPL,  and)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_OR_IMPL,   or)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_XOR_IMPL,  xor)
ITP_INSN(ITP_TYPE_ALU_REG, ITP_FORMAT_ALU_REG, ITP_NOR_IMPL,  nor)

ITP_INSN(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLTZ_IMPL, bltz)
ITP_INSN(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGEZ_IMPL, bgez)
ITP_INSN(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLEZ_IMPL, blez)
ITP_INSN(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGTZ_IMPL, bgtz)
ITP_INSN(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BLTZAL_IMPL, bltzal)
ITP_INSN(ITP_TYPE_BRANCH_GENERIC, ITP_FORMAT_BRANCH_GENERIC, ITP_BGEZAL_IMPL, bgezal)

ITP_INSN(ITP_TYPE_J_JAL, ITP_FORMAT_J_JAL, ITP_J_IMPL, j)
ITP_INSN(ITP_TYPE_J_JAL, ITP_FORMAT_J_JAL, ITP_JAL_IMPL, jal)

ITP_INSN(ITP_TYPE_BRANCH_EQ_NE, ITP_FORMAT_BRANCH_EQ_NE, ITP_BEQ_IMPL, beq)
ITP_INSN(ITP_TYPE_BRANCH_EQ_NE, ITP_FORMAT_BRANCH_EQ_NE, ITP_BNE_IMPL, bne)

ITP_INSN(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ADDI_IMPL,  addi)
ITP_INSN(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ADDIU_IMPL, addiu)
ITP_INSN(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_SLTI_IMPL,  slti)
ITP_INSN(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_SLTIU_IMPL, sltiu)
ITP_INSN(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ANDI_IMPL,  andi)
ITP_INSN(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_ORI_IMPL,   ori)
ITP_INSN(ITP_TYPE_ALU_IMM, ITP_FORMAT_ALU_IMM, ITP_XORI_IMPL,  xori)

ITP_INSN(ITP_TYPE_LUI_IMM, ITP_FORMAT_LUI_IMM, ITP_LUI_IMPL, lui)

ITP_INSN(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LB_IMPL,  lb)
ITP_INSN(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LH_IMPL,  lh)
ITP_INSN(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LWL_IMPL, lwl)
ITP_INSN(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LW_IMPL,  lw)
ITP_INSN(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LBU_IMPL, lbu)
ITP_INSN(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LHU_IMPL, lhu)
ITP_INSN(ITP_TYPE_LOAD_IMM, ITP_FORMAT_LOAD_IMM, ITP_LWR_IMPL, lwr)

ITP_INSN(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SB_IMPL,  sb)
ITP_INSN(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SH_IMPL,  sh)
ITP_INSN(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SWL_IMPL, swl)
ITP_INSN(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SW_IMPL,  sw)
ITP_INSN(ITP_TYPE_STORE_IMM, ITP_FORMAT_STORE_IMM, ITP_SWR_IMPL, swr)

void interpreter_switch(struct mips *mips, struct memory *memory) {
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
    case 0x0: 
        goto secondary_op;
    case 0x1:    
        goto    branch_op;

    case 0x02:  interpret_j(mips, memory);       return;
    case 0x03:  interpret_jal(mips, memory);     return;
    case 0x04:  interpret_beq(mips, memory);     return;
    case 0x05:  interpret_bne(mips, memory);     return;
    case 0x06:  interpret_blez(mips, memory);    return;
    case 0x07:  interpret_bgtz(mips, memory);    return;
    case 0x08:  interpret_addi(mips, memory);    return;
    case 0x09:  interpret_addiu(mips, memory);   return;
    case 0x0a:  interpret_slti(mips, memory);    return;
    case 0x0b:  interpret_sltiu(mips, memory);   return;
    case 0x0c:  interpret_andi(mips, memory);    return;
    case 0x0d:  interpret_ori(mips, memory);     return;
    case 0x0e:  interpret_xori(mips, memory);    return;
    case 0x0f:  interpret_lui(mips, memory);     return;
    case 0x20:  interpret_lb(mips, memory);      return;
    case 0x21:  interpret_lh(mips, memory);      return;
    case 0x22:  interpret_lwl(mips, memory);     return;
    case 0x23:  interpret_lw(mips, memory);      return;
    case 0x24:  interpret_lbu(mips, memory);     return;
    case 0x25:  interpret_lhu(mips, memory);     return;
    case 0x26:  interpret_lwr(mips, memory);     return;
    case 0x28:  interpret_sb(mips, memory);      return;
    case 0x29:  interpret_sh(mips, memory);      return;
    case 0x2a:  interpret_swl(mips, memory);     return;
    case 0x2b:  interpret_sw(mips, memory);      return;
    case 0x2e:  interpret_swr(mips, memory);     return;
    default:
        assert(0 && "Unknown primary instruction");
    }
secondary_op:
    switch((mips->r[MIPS_R_CIR] >> FN_SHIFT) & 
                                   FN_MASK) {
    case 0x00: interpret_sll(mips, memory);      return;
    case 0x02: interpret_srl(mips, memory);      return;
    case 0x03: interpret_sra(mips, memory);      return;
    case 0x04: interpret_sllv(mips, memory);     return;
    case 0x06: interpret_srlv(mips, memory);     return;
    case 0x07: interpret_srav(mips, memory);     return;
    case 0x08: interpret_jr(mips, memory);       return;
    case 0x09: interpret_jalr(mips, memory);     return;
    case 0x0c: interpret_syscall(mips, memory);  return;
    case 0x0d: interpret_brk(mips, memory);      return;
    case 0x10: interpret_mfhi(mips, memory);     return;
    case 0x11: interpret_mthi(mips, memory);     return;
    case 0x12: interpret_mflo(mips, memory);     return;
    case 0x13: interpret_mtlo(mips, memory);     return;
    case 0x18: interpret_mult(mips, memory);     return;
    case 0x19: interpret_multu(mips, memory);    return;
    case 0x1a: interpret_div(mips, memory);      return;
    case 0x1b: interpret_divu(mips, memory);     return;
    case 0x20: interpret_add(mips, memory);      return;
    case 0x21: interpret_addu(mips, memory);     return;
    case 0x22: interpret_sub(mips, memory);      return;
    case 0x23: interpret_subu(mips, memory);     return;
    case 0x24: interpret_and(mips, memory);      return;
    case 0x25: interpret_or(mips, memory);       return;
    case 0x26: interpret_xor(mips, memory);      return;
    case 0x27: interpret_nor(mips, memory);      return;
    case 0x2a: interpret_slt(mips, memory);      return;
    case 0x2b: interpret_sltu(mips, memory);     return;
    default:
        assert(0 && "Unknown secondary instruction");
    }
branch_op:
    switch((mips->r[MIPS_R_CIR] >> RT_SHIFT) & 
                                   RT_MASK) {
    case 0x00: interpret_bltz(mips, memory);     return;
    case 0x01: interpret_bgez(mips, memory);     return;
    case 0x10: interpret_bltzal(mips, memory);   return;
    case 0x11: interpret_bgezal(mips, memory);   return;
    default:
        assert(0 && "Unknown branch instruction");
    }
}

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

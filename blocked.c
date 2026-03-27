#include "common.h"
#include "log.h"

#include "mips.h"
#include "memory.h"
#include "loader.h"
#include "instructions.h"

#ifdef __DISASSEMBLE__
#define ITP_INSN(type, formatter, impl, name)           \
    static void interpret_##name(struct mips *mips,     \
                          struct memory *memory) {      \
        type                                            \
        formatter(#name)                                \
        impl                                            \
    }
#else // __DISASSEMBLE__
#define ITP_INSN(type, formatter, impl, name)          \
    static void interpret_##name(struct mips *mips,    \
                          struct memory *memory) {     \
        type                                           \
        impl                                           \
    }
#endif // __DISASSEMBLE__

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
ITP_INSN(ITP_TYPE_BRK, ITP_FORMAT_SYSCALL, ITP_BRK_IMPL, brk)

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
ITP_INSN(ITP_TYPE_RDHWR, ITP_FORMAT_RDHWR, ITP_RDHWR_IMPL, rdhwr)

#define DISPATCH                                  \
    memory_read(memory, pc, &cir, 4);             \
    pc += 4;                                      \
    goto *primary[                                \
        (cir >> OP_SHIFT) & OP_MASK];

#define DARRAY_PUSH(ops, function)                \
    do {                                          \
        if (size == capacity) {                   \
            capacity *= 2;                        \
            ops = realloc(                        \
                ops, sizeof(*ops) * capacity);    \
        }                                         \
        ops[size++] = (op_t) {                    \
            .cir = cir, .op = function            \
        };                                        \
    } while(0)

#define LABEL(name, function)                     \
    do_ ## name:                                  \
        DARRAY_PUSH(ops, &function);              \
        if (!branched && !mips->halted) {         \
            DISPATCH;                             \
        }                                         \
        goto complete

#define LABEL_BRANCH(name, function)              \
    do_ ## name:                                  \
        DARRAY_PUSH(ops, &function);              \
        branched = 1;                             \
        DISPATCH

typedef struct {
    u32 cir;
    void (*op)
        (struct mips*, struct memory *);
} op_t;

typedef struct block {
    u32 address;                            /* key + start address of block  */
    u32 size;                               /* number of operations in block */
    op_t *ops;                              /* operations in a block         */

    UT_hash_handle hh;                      /* make this structure hashable  */
} block_t;

static block_t *decode_block(struct mips   *mips,
                             struct memory *memory) {

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
        [LBU_OP]     = &&do_lbu,     [LHU_OP]    = &&do_lhu,
        [LWR_OP]     = &&do_lwr,     [SB_OP]     = &&do_sb,
        [SH_OP]      = &&do_sh,      [SWL_OP]    = &&do_swl,
        [SW_OP]      = &&do_sw,      [SWR_OP]    = &&do_swr,
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
    };
    static const void *branch[] = {
        [BLTZ_RT]    = &&do_bltz,    [BGEZ_RT]   = &&do_bgez,
        [BLTZAL_RT]  = &&do_bltzal,  [BGEZAL_RT] = &&do_bgezal,
    };

    // create block variable 
    block_t *blk = NULL;
    
    // branched flag
    u32 branched = 0;
    
    // processor registers 
    u32 pc = mips->r[MIPS_R_PC], cir = 0;

    // ops dynamic array values;
    u32  size = 0, capacity = 32;
    op_t *ops = malloc(sizeof(*ops) * capacity);

    for (;;) {
        /* start dispatch chain */
        DISPATCH;
        
do_secondary: goto *secondary[(cir >> FN_SHIFT) & FN_MASK];
do_branch:    goto    *branch[(cir >> RT_SHIFT) & RT_MASK];

        /*========== branching instructions ===========*/
        LABEL_BRANCH(jr,     interpret_jr);
        LABEL_BRANCH(jalr,   interpret_jalr);
        LABEL_BRANCH(bltz,   interpret_bltz);
        LABEL_BRANCH(bgez,   interpret_bgez);
        LABEL_BRANCH(blez,   interpret_blez);
        LABEL_BRANCH(bgtz,   interpret_bgtz);
        LABEL_BRANCH(bltzal, interpret_bltzal);
        LABEL_BRANCH(bgezal, interpret_bgezal);
        LABEL_BRANCH(j,      interpret_j);
        LABEL_BRANCH(jal,    interpret_jal);
        LABEL_BRANCH(beq,    interpret_beq);
        LABEL_BRANCH(bne,    interpret_bne);

        /*============ basic instructions =============*/
        LABEL(syscall,       interpret_syscall);
        LABEL(brk,           interpret_brk);
        LABEL(sll,           interpret_sll);
        LABEL(srl,           interpret_srl);
        LABEL(sra,           interpret_sra);
        LABEL(sllv,          interpret_sllv);
        LABEL(srlv,          interpret_srlv);
        LABEL(srav,          interpret_srav);
        LABEL(mfhi,          interpret_mfhi);
        LABEL(mflo,          interpret_mflo);
        LABEL(mthi,          interpret_mthi);
        LABEL(mtlo,          interpret_mtlo);
        LABEL(mult,          interpret_mult);
        LABEL(multu,         interpret_multu);
        LABEL(div,           interpret_div);
        LABEL(divu,          interpret_divu);
        LABEL(add,           interpret_add);
        LABEL(addu,          interpret_addu);
        LABEL(sub,           interpret_sub);
        LABEL(subu,          interpret_subu);
        LABEL(slt,           interpret_slt);
        LABEL(sltu,          interpret_sltu);
        LABEL(and,           interpret_and);
        LABEL(or,            interpret_or);
        LABEL(xor,           interpret_xor);
        LABEL(nor,           interpret_nor);
        LABEL(addi,          interpret_addi);
        LABEL(addiu,         interpret_addiu);
        LABEL(slti,          interpret_slti);
        LABEL(sltiu,         interpret_sltiu);
        LABEL(andi,          interpret_andi);
        LABEL(ori,           interpret_ori);
        LABEL(xori,          interpret_xori);
        LABEL(lui,           interpret_lui);
        LABEL(lb,            interpret_lb);
        LABEL(lh,            interpret_lh);
        LABEL(lwl,           interpret_lwl);
        LABEL(lw,            interpret_lw);
        LABEL(lbu,           interpret_lbu);
        LABEL(lhu,           interpret_lhu);
        LABEL(lwr,           interpret_lwr);
        LABEL(sb,            interpret_sb);
        LABEL(sh,            interpret_sh);
        LABEL(swl,           interpret_swl);
        LABEL(sw,            interpret_sw);
        LABEL(swr,           interpret_swr);
        LABEL(rdhwr,         interpret_rdhwr);
    }

complete:
    
    blk = malloc(sizeof(*blk));
    blk->address = mips->r[MIPS_R_PC];
    blk->size    = size;
    blk->ops     = ops;

    return blk;
}

#ifndef __MACRO_EXPANSION__
void interpreter_blocked(struct mips   *mips, 
                         struct memory *memory) {
    block_t *blocks = NULL;

    for (; !mips->halted ;) {
        block_t *blk = NULL;
 
        HASH_FIND(hh, blocks, 
            &mips->r[MIPS_R_PC], sizeof(u32), blk);

        if (blk == NULL) {
            blk = decode_block(mips, memory);

            HASH_ADD(hh, blocks, 
                address, sizeof(u32), blk);
        }

        for (u32 s = 0; s < blk->size; s++) {
            op_t o = blk->ops[s];
            mips->r[MIPS_R_CIR] = o.cir;
            o.op(mips, memory);
            INCREMENT_PC;
        }
    }

    block_t *b, *t;
    HASH_ITER(hh, blocks, b, t) {
        HASH_DEL(blocks, b);
        free(b->ops);
        free(b);
    }
}


#ifdef __BLOCKED_MAIN__
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s mips.elf\n", *argv);
        return 1;
    }

    /* create emulator resources */
    struct mips   mips   = {};
    struct memory memory = {};

    /* handle any explicit object creation */
    create_memory(&memory);
    
    /* load elf into memory */
    loader_elf(&mips, &memory, *++argv);
    
    /* start the decoupled interpreter */
    interpreter_blocked(&mips, &memory);

    /* clean up */
    delete_memory(&memory);

    return 0;
}
#endif // __BLOCKED_MAIN__
#endif // __MACRO_EXPANSION__

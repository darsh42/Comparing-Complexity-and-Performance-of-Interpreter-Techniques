#include "common.h"
#include "log.h"

#include "mips.h"
#include "memory.h"
#include "loader.h"

#include "benchmark.h"
#include "instructions.h"

#define UNROLL_SIZE 8

typedef struct op op_t;

typedef void (*handler) (u32, 
     struct mips   * restrict, 
     struct memory * restrict);

typedef struct op {
    u32     cir;
    handler op;
} op_t;

typedef struct block {
    u32  address;
    u32  size;
    op_t *ops;
    bool indirect;

    struct block *next[2]; // 0 - continue
                           // 1 - jump/branch

    UT_hash_handle hh;
} block_t;

#if    defined(__DISASSEMBLE__)
#define X(prologue, formatter, implementation, value, name)            \
    static void interpret_##name(u32 cir,                              \
                                 struct mips   * restrict mips,        \
                                 struct memory * restrict memory){     \
        prologue                                                       \
        formatter(#name)                                               \
        implementation                                                 \
        mips->r[MIPS_R_PC] += 4;                                       \
    }
#elif  defined(__PROFILE__)
#define X(prologue, formatter, implementation, value, name)            \
    static void interpret_##name(u32 cir,                              \
                                 struct mips   * restrict mips,        \
                                 struct memory * restrict memory){     \
        PROFILE_ENTER_INSTRUCTION                                      \
        prologue                                                       \
        implementation                                                 \
        PROFILE_EXIT_INSTRUCTION                                       \
    }
#else
#define X(prologue, formatter, implementation, value, name)            \
    static void interpret_##name(u32 cir,                              \
                                 struct mips   * restrict mips,        \
                                 struct memory * restrict memory){     \
        prologue                                                       \
        implementation                                                 \
    }
#endif

__INSTRUCTIONS

#undef X

#define DISPATCH                                  \
    memory_read_u32(memory, pc, &cir);            \
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

#define LABEL_BRANCH_INDIRECT(name, function)     \
    do_ ## name:                                  \
        DARRAY_PUSH(ops, &function);              \
        branched = 1;                             \
        indirect = 1;                             \
        DISPATCH

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
        [LL_OP]      = &&do_ll,      [LBU_OP]    = &&do_lbu,
        [LHU_OP]     = &&do_lhu,     [LWR_OP]    = &&do_lwr,
        [SH_OP]      = &&do_sh,      [SWL_OP]    = &&do_swl,
        [SW_OP]      = &&do_sw,      [SWR_OP]    = &&do_swr,
        [SB_OP]      = &&do_sb,      [SC_OP]     = &&do_sc,
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

    // create block variable 
    block_t *blk = NULL;
    
    // branched flag
    u32 branched = 0;
    u32 indirect = 0;
    
    // processor registers 
    u32 pc = mips->r[MIPS_R_PC], cir;

    // ops dynamic array values;
    u32  size = 0, capacity = 32;
    op_t *ops = malloc(sizeof(*ops) * capacity);

    /* start dispatch chain */
    DISPATCH;
    
do_secondary: goto *secondary[(cir >> FN_SHIFT) & FN_MASK];
do_branch:    goto    *branch[(cir >> RT_SHIFT) & RT_MASK];

    /*========== indirect branching instructions ===========*/
    LABEL_BRANCH_INDIRECT(jr,   interpret_jr);
    LABEL_BRANCH_INDIRECT(jalr, interpret_jalr);

    /*========== branching instructions ===========*/
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
    LABEL(special,       interpret_special);
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
    LABEL(ll,            interpret_ll);
    LABEL(lbu,           interpret_lbu);
    LABEL(lhu,           interpret_lhu);
    LABEL(lwr,           interpret_lwr);
    LABEL(sb,            interpret_sb);
    LABEL(sh,            interpret_sh);
    LABEL(swl,           interpret_swl);
    LABEL(sw,            interpret_sw);
    LABEL(sc,            interpret_sc);
    LABEL(swr,           interpret_swr);
    LABEL(rdhwr,         interpret_rdhwr);

complete:
    // append null terminator
    DARRAY_PUSH(ops, NULL);

    blk = malloc(sizeof(*blk));
    blk->address  = mips->r[MIPS_R_PC];
    blk->size     = size;
    blk->ops      = ops;
    blk->indirect = indirect;

    blk->next[0]  = NULL;
    blk->next[1]  = NULL;

    return blk;
}

void interpreter_blocked(struct mips   * restrict mips,
                         struct memory * restrict memory) {
    block_t *blocks = NULL, *previous = NULL;

    while (!mips->halted) {
        block_t *current = NULL;
        
        HASH_FIND(hh, blocks, 
            &mips->r[MIPS_R_PC], sizeof(u32), current);

        if (current == NULL) {
            // decode new block
            current = decode_block(mips, memory);
            
            // add to hash table
            HASH_ADD(hh, blocks, 
                address, sizeof(u32), current);
        }

        // check if there is a previous block and link
        if (previous != NULL && !previous->indirect) {
            previous->next[mips->branched] = current;
        }

        do {
#ifndef    __DISASSEMBLE__
            // if not disassembling, pre-increment
            // program counter to value at branch
            mips->r[MIPS_R_PC] += 
                4*(current->size - 3);
#endif // __DISASSEMBLE__

            // get ops local
            op_t     *ops  = current->ops;
            block_t **link = current->next;

            // prefetch next blocks
            __builtin_prefetch(link[0], 0, 1);
            __builtin_prefetch(link[1], 0, 1);

            switch((current->size-1)% UNROLL_SIZE) {
                case 0: do {
                case 8: ops->op(ops->cir, mips, memory); ops++;
                case 7: ops->op(ops->cir, mips, memory); ops++;
                case 6: ops->op(ops->cir, mips, memory); ops++;
                case 5: ops->op(ops->cir, mips, memory); ops++;
                case 4: ops->op(ops->cir, mips, memory); ops++;
                case 3: ops->op(ops->cir, mips, memory); ops++;
                case 2: ops->op(ops->cir, mips, memory); ops++;
                case 1: ops->op(ops->cir, mips, memory); ops++;
                } while(ops->op);
            }

            mips->r[MIPS_R_PC] = 
                mips->r[MIPS_R_NNPC];
    
            // set new block pointers
            previous = current;
            current  = link[mips->branched];
        } while (current && !previous->indirect && !mips->halted);
    }

    block_t *b, *t;
    HASH_ITER(hh, blocks, b, t) {
        HASH_DEL(blocks, b);
        free(b->ops);
        free(b);
    }
}

#ifndef __MACRO_EXPANSION__
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
    memory_create(&memory);
    
    /* load elf into memory */
    loader_elf(&mips, &memory, *++argv);

#ifdef    __PROFILE__
    PROFILE_ENTER_INTERPRETER
#endif // __PROFILE__
    /* start the decoupled interpreter */
    interpreter_blocked(&mips, &memory);
#ifdef    __PROFILE__
    PROFILE_EXIT_INTERPRETER
#endif // __PROFILE__

    /* clean up */
    memory_delete(&memory);

    return 0;
}
#endif // __BLOCKED_MAIN__
#endif // __MACRO_EXPANSION__

#include "common.h"
#include "log.h"

#include "mips.h"
#include "memory.h"
#include "loader.h"
#include "idecode.h"
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

ITP_INSN(ITP_TYPE_NONE, ITP_FORMAT_NONE, ITP_BRANCH_DELAY_IMPL, branch_delay)
ITP_INSN(ITP_TYPE_NONE, ITP_FORMAT_NONE, ITP_HALT_IMPL, halt)

#define PREFETCH_SIZE ((CACHELINE_SIZE / sizeof(void *))+1)
#define EXECUTER_SIZE  (CACHELINE_SIZE / sizeof(void *))

#define DISPATCH                                                \
    goto *primary_opcode_labels[                                \
        (*prefetch >> OP_SHIFT) & OP_MASK];

#define LABEL(name, function)                                   \
    do_ ## name:                                                \
        /* increment the filled count */                        \
        (*filled)++;                                            \
        /* push function into cache temporary */                \
        *decoded++ = (void *) &function;                        \
        /* check and decrement remaining */                     \
        if (remaining-- > 0) {                                  \
            /* increment prefetcher */                          \
            prefetch++;                                         \
            /* dispatch next label */                           \
            DISPATCH;                                           \
        }                                                       \
        goto complete;
#define LABEL_BRANCH(name, function)                            \
    do_ ## name:                                                \
        /* increment the filled count */                        \
        (*filled)++;                                            \
        /* push function into cache temporary */                \
        *decoded++ = (void *) &function;                        \
        /* set the block kind */                                \
        result = DECODE_BRANCH;                                 \
        /* set change remaining to do branch delay */           \
        remaining = 1;                                          \
        /* increment prefetcher */                              \
        prefetch++;                                             \
        /* dispatch next label */                               \
        DISPATCH;
#define LABEL_HALT(name, function)                              \
    do_ ## name:                                                \
        /* increment the filled count */                        \
        (*filled)++;                                            \
        /* push function into decoded pointer array*/           \
        *decoded++ = (void *) function;                         \
        /* set the halted decode result */                      \
        result = DECODE_HALT;                                   \
        /* goto complete label */                               \
        goto complete;

typedef struct {
    struct mips   *mips;
    struct memory *memory;
    
    idecode_queue_t *id;
} decode_args_t;

typedef struct {
    struct mips   *mips;
    struct memory *memory;
    
    idecode_queue_t *id;
} execute_args_t;

typedef enum {
    DECODE_CONTINUE,
    DECODE_BRANCH,
    DECODE_HALT
} decode_result_t;

typedef void (*instruction)
    (struct mips*, struct memory*);

decode_result_t decode_block(void **decoded, u32 *prefetch, u32 *filled) {
    decode_result_t result = DECODE_CONTINUE;

    /* keep a single instruction padding *
     * this guards agains the case where *
     * the last instruction in the pref- *
     * etcher is a branch as then we can *
     * complete the load delay           */
    u32 remaining = PREFETCH_SIZE - 1;

    /* define each label array */                                
    void *primary_opcode_labels[] = {
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
    };
    void *secondary_opcode_labels[] = {
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
    void *branch_opcode_labels[] = {
        [BLTZ_RT]    = &&do_bltz,    [BGEZ_RT]   = &&do_bgez,
        [BLTZAL_RT]  = &&do_bltzal,  [BGEZAL_RT] = &&do_bgezal,
    };

    /* start dispatch chain */
    DISPATCH;
    
do_secondary:
    goto *secondary_opcode_labels[
        (*prefetch >> FN_SHIFT) & FN_MASK];
do_branch:
    goto *branch_opcode_labels[
        (*prefetch >> RT_SHIFT) & RT_MASK];

    /*============= halting instructions ==============*/
    LABEL_HALT(brk,      interpret_brk)

    /*========== branching instructions ===========*/
    LABEL_BRANCH(jr,     interpret_jr)
    LABEL_BRANCH(jalr,   interpret_jalr)
    LABEL_BRANCH(bltz,   interpret_bltz)
    LABEL_BRANCH(bgez,   interpret_bgez)
    LABEL_BRANCH(blez,   interpret_blez)
    LABEL_BRANCH(bgtz,   interpret_bgtz)
    LABEL_BRANCH(bltzal, interpret_bltzal)
    LABEL_BRANCH(bgezal, interpret_bgezal)
    LABEL_BRANCH(j,      interpret_j)
    LABEL_BRANCH(jal,    interpret_jal)
    LABEL_BRANCH(beq,    interpret_beq)
    LABEL_BRANCH(bne,    interpret_bne)

    /*============ basic instructions =============*/
    LABEL(syscall,       interpret_syscall)
    LABEL(sll,           interpret_sll)
    LABEL(srl,           interpret_srl)
    LABEL(sra,           interpret_sra)
    LABEL(sllv,          interpret_sllv)
    LABEL(srlv,          interpret_srlv)
    LABEL(srav,          interpret_srav)
    LABEL(mfhi,          interpret_mfhi)
    LABEL(mflo,          interpret_mflo)
    LABEL(mthi,          interpret_mthi)
    LABEL(mtlo,          interpret_mtlo)
    LABEL(mult,          interpret_mult)
    LABEL(multu,         interpret_multu)
    LABEL(div,           interpret_div)
    LABEL(divu,          interpret_divu)
    LABEL(add,           interpret_add)
    LABEL(addu,          interpret_addu)
    LABEL(sub,           interpret_sub)
    LABEL(subu,          interpret_subu)
    LABEL(slt,           interpret_slt)
    LABEL(sltu,          interpret_sltu)
    LABEL(and,           interpret_and)
    LABEL(or,            interpret_or)
    LABEL(xor,           interpret_xor)
    LABEL(nor,           interpret_nor)
    LABEL(addi,          interpret_addi)
    LABEL(addiu,         interpret_addiu)
    LABEL(slti,          interpret_slti)
    LABEL(sltiu,         interpret_sltiu)
    LABEL(andi,          interpret_andi)
    LABEL(ori,           interpret_ori)
    LABEL(xori,          interpret_xori)
    LABEL(lui,           interpret_lui)
    LABEL(lb,            interpret_lb)
    LABEL(lh,            interpret_lh)
    LABEL(lwl,           interpret_lwl)
    LABEL(lw,            interpret_lw)
    LABEL(lbu,           interpret_lbu)
    LABEL(lhu,           interpret_lhu)
    LABEL(lwr,           interpret_lwr)
    LABEL(sb,            interpret_sb)
    LABEL(sh,            interpret_sh)
    LABEL(swl,           interpret_swl)
    LABEL(sw,            interpret_sw)
    LABEL(swr,           interpret_swr)

complete:
    return result;
}

void *decode(void *args) {
    /*
     *   ----------------------------------------------------------------
     *  |   committed    |   committing   |         empty/old           |
     *
     *  transform computed goto to decoder and executor pipeline
     *      compute function pointers in blocks
     *      get return addresses in array 
     *      do binary search based on address returns
     *      handle branch delays 
     *
     *  apply caching to the pipelined infrastructure
     *      group 'hot' addresses 
     *      create cache blocks 
     *      make decoder check if current cache block is cached
     *      dispatch current cache block 
     *      decode from end of cache block
     *
     * */

    // pointer to emulator resources
    struct mips   *mips =
        ((decode_args_t*) args)->mips;
    struct memory *memory =
        ((decode_args_t*) args)->memory;

    // instruction decoding queue
    idecode_queue_t *id =
        ((decode_args_t*) args)->id;

    // data used in decode queue
    u32  prefetch[PREFETCH_SIZE];
    void *decoded[PREFETCH_SIZE];

    // decoder program counter
    u32 pc;

    /* decided on a goto based algorithm to 
     * reduce the number of checks and jumps
     *
     * below is a pseudo-code break down of 
     * the options, in addition I have counted
     * the number of jumps within the loop and 
     * the number of conditional checks
     *
     * Idea is for hot code path to be extreemly
     * fast and the branch and halt path to be 
     * slower 
     *
     * OPTION 1 - Jumps=2, Cond=2:
     *  while (k != BLOCK_HALT) {
     *      k = decode();
     *      if (k == BLOCK_BRANCH) {
     *          complete branch
     *      }
     *  }
     *  complete halt
     *  
     *  Option 2 - Jumps=2, Cond=1:
     *  for (;;) {
     *      k = decode();
     *      switch (k) {
     *      case BLOCK_BRANCH: 
     *          complete branch; 
     *          break
     *      case BLOCK_HALT:   
     *          complete halt; 
     *          return k;
     *      }
     *  }
     *
     *  Option 3 - Jumps=1, Cond=1:
     *  resume:
     *  while (k == BLOCK_BASIC) {
     *      k = decode();
     *  }
     *  if (k == BLOCK_BRANCH) {
     *      complete branch; goto resume;
     *  }
     *  complete halt;
     *  return k;
     *  */

    // handle decoding
    decode_result_t result;
    do {
        // ensure the decode queue is empty
        idecode_wait_taken(id, 0);
            
        /* retrive pc */
        pc = mips->r[MIPS_R_PC];

        do {
            // define filled
            u32 filled = 0;

            // read instructions 
            memory_read(memory, pc, 
                prefetch, PREFETCH_SIZE);

            // decode instructions
            result = decode_block(
                decoded, prefetch, &filled);

            // enqueue all instructions
            idecode_batch_enqueue_soa(
                id, prefetch, decoded, filled);
        } while(result == DECODE_CONTINUE);
        
        // handle branch
        if (result == DECODE_BRANCH) {
            // wait untill space for branch delay
            idecode_wait_space(id, 1);

            // enqueue branch delay
            idecode_enqueue_soa(id, 0, 
                (void *) &interpret_branch_delay);
        }
    } while (result != DECODE_HALT);

    // wait until space for branch delay
    idecode_wait_space(id, 1);

    // handle halt
    idecode_enqueue_soa(id, 0,
        (void *) &interpret_halt);

    return NULL;
}

void *execute(void *args) {
    // pointer to emulator resources
    struct mips   *mips =
        ((execute_args_t*) args)->mips;
    struct memory *memory =
        ((execute_args_t*) args)->memory;

    // instruction decoding queue
    idecode_queue_t *id =
        ((execute_args_t*) args)->id;

    // predefine BATCH size of executor
    idecode_item_t 
        items[EXECUTER_SIZE];

    while (!mips->halted) {
        u32 filled_slots = 
            idecode_batch_dequeue(
                id, items, EXECUTER_SIZE);
        
        // execute decoded instructions
        for (u32 i = 0; i < filled_slots; i++) {
            mips->r[MIPS_R_PC] += 4;
            mips->r[MIPS_R_CIR] = items[i].opcode;
            
            /* call instruction */
            ((instruction) items[i].handle)(mips, memory);
        }
    }

    return NULL;
}

void interpreter_decoupled(struct mips *mips, struct memory *memory) {
    idecode_queue_t idecode = {};

    /* create thread arguments */
    execute_args_t execute_args = {
        .mips = mips, .memory = memory, .id = &idecode };
    decode_args_t decode_args = {
        .mips = mips, .memory = memory, .id = &idecode };

    /* define threads */
    pthread_t thread_execute;
    pthread_t thread_decode;

    /* spawn threads create execute first since it will *
     * poll for new instructions depending decode       */
    assert(!pthread_create(&thread_execute, NULL, 
        execute, (void *) &execute_args));
    assert(!pthread_create(&thread_decode , NULL, 
        decode, (void *) &decode_args));

    /* join threads for exit */
    pthread_join(thread_decode , NULL);
    pthread_join(thread_execute, NULL);
}

#ifdef __DECOUPLED_MAIN__
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
    interpreter_decoupled(&mips, &memory);

    /* clean up */
    delete_memory(&memory);

    return 0;
}
#endif // __DECOUPLED_MAIN__

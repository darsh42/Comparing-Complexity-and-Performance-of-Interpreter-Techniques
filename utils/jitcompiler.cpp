/*****************************************************
 *                   jitcompiler                     *
 *****************************************************
 * One of the emulation techniques tested in this i- *
 * nvestigation                                      *
 *****************************************************/
#include <map>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <elf.h>

#include "asmjit/x86.h"

#include "log.h"
#include "mips.h"
#include "memory.h"
#include "loader.h"

#include "interpreter_templates.h"
#include "jitcompiler_templates.h"

#define ERR_CHECK(expr, msg)                       \
    if ((err = (expr)) != Error::kOk) {            \
        printf("ERROR: " msg " %s\n",              \
                DebugUtils::error_as_string(err)); \
    }


// clang-format off
block jitcompile(asmjit::JitRuntime *runtime, struct memory *memory, u32 start_pc) {
    /* When a "block" has reached a specific hot count *
     * it is compiled.                                 *
     *                                                 *
     * The compilation is done by creating a function. *
     * The function is constructed of multiple instru- *
     * ctions.                                         *
     *                                                 *
     * The compiler will take the start address of     *
     * the block and iterate through each subsequent   *
     * instruction until it encounters a NEW_BLOCK     *
     * instruction.                                    */
    using namespace asmjit;

    /* error holder */
    Error err;

    /* create and initialise the code holder */
    CodeHolder code;
    code.init(runtime->environment(),
              runtime->cpu_features());

    /* create the compiler and block */
    x86::Compiler cc(&code);
    FuncNode *block_node = cc.add_func(
            FuncSignature::build<void,struct mips*,struct memory*>());
    
    /* create arg pointers */
    x86::Gp jit_mips   = cc.new_gp_ptr("jit_mips");
    x86::Gp jit_memory = cc.new_gp_ptr("jit_memory");

    /* retrieve args */
    block_node->set_arg(0, jit_mips);
    block_node->set_arg(1, jit_memory);

    /* start building the block */
    u32 pc = start_pc, cir;
    for(;;) {
        memory_read(memory, pc, &cir, 4);
        switch((cir >> OP_SHIFT) & OP_MASK) {
        default: 
            goto block_finished;
        case 0x00: 
            goto secondary_op;
        case 0x08: compile_addi   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x09: compile_addiu  (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x0a: compile_slti   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x0b: compile_sltiu  (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x0c: compile_andi   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x0d: compile_ori    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x0e: compile_xori   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x0f: compile_lui    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x20: compile_lb     (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x21: compile_lh     (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x22: compile_lwl    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x23: compile_lw     (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x24: compile_lbu    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x25: compile_lhu    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x26: compile_lwr    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x28: compile_sb     (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x29: compile_sh     (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x2a: compile_swl    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x2b: compile_sw     (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x2e: compile_swr    (&cc, jit_mips, jit_memory, pc, cir); break;
        } goto opcode_finished;
    secondary_op:
        switch((cir >> FN_SHIFT) & FN_MASK) {
        default: 
            goto block_finished;
        case 0x00: compile_sll    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x02: compile_srl    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x03: compile_sra    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x04: compile_sllv   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x06: compile_srlv   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x07: compile_srav   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x10: compile_mfhi   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x11: compile_mthi   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x12: compile_mflo   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x13: compile_mtlo   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x18: compile_mult   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x19: compile_multu  (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x1a: compile_div    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x1b: compile_divu   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x20: compile_add    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x21: compile_addu   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x22: compile_sub    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x23: compile_subu   (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x24: compile_and    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x25: compile_or     (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x26: compile_xor    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x27: compile_nor    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x2a: compile_slt    (&cc, jit_mips, jit_memory, pc, cir); break;
        case 0x2b: compile_sltu   (&cc, jit_mips, jit_memory, pc, cir); break;
        } goto opcode_finished;
    opcode_finished:
        /* increment the interpreted pc */
        pc += 4;
    };
block_finished:
    /* increment jitted pc by pc - start_pc */
    cc.add(access_r_static(MIPS_R_PC), 
           imm(pc - start_pc));

    /* complete the block */
    cc.end_func();
    cc.finalize();

    /* add the block to the run time */
    block b;
    ERR_CHECK(runtime->add(&b, &code),
            "Unable to add block");

    /* return the block */
    return b;
}

enum MIPS_SECTION interpret(struct mips *mips, struct memory *memory) {
    /* Determine if the next instruction will be    *
     * entering a new 'block'                       *
     *                                              *
     * blocks are the smallest unit of work         *
     * the jit compiler will compile.               *
     *                                              *
     * A block is started and concluded whenever    *
     * a jump or branch instruction is encountered. *
     *                                              *
     * The minimal implementation can check for on- *
     * ly branches.                                 *
     *                                              *
     * INSTRUCTIONS:                                *
     *   primary:   j  jal beq bne blez bgtz        *
     *   secondary: jr jalr                         *
     *   branch:    bltz bgez bltzal bgezal         *
     *                                              *
     * backtracking, some instructions, mainly      *
     * branches will change block bounderies. When  *
     * a branch op is encountered, it can branch to *
     * a previous instruction, this is where it the *
     * branch block will then start.                *
     *
     * Would it be a fair assumption that branch    *
     * instructions that need to be compiled will   *
     * be 'hotter' than the calling function?       */

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
    case 0x0: goto secondary_op;
    case 0x1: goto    branch_op;

    case 0x02:  interpret_j(mips, memory);      return NEW_BLOCK;
    case 0x03:  interpret_jal(mips, memory);    return NEW_BLOCK;
    case 0x04:  interpret_beq(mips, memory);    return NEW_BLOCK;
    case 0x05:  interpret_bne(mips, memory);    return NEW_BLOCK;
    case 0x06:  interpret_blez(mips, memory);   return NEW_BLOCK;
    case 0x07:  interpret_bgtz(mips, memory);   return NEW_BLOCK;
    case 0x08:  interpret_addi(mips, memory);   return OLD_BLOCK;
    case 0x09:  interpret_addiu(mips, memory);  return OLD_BLOCK;
    case 0x0a:  interpret_slti(mips, memory);   return OLD_BLOCK;
    case 0x0b:  interpret_sltiu(mips, memory);  return OLD_BLOCK;
    case 0x0c:  interpret_andi(mips, memory);   return OLD_BLOCK;
    case 0x0d:  interpret_ori(mips, memory);    return OLD_BLOCK;
    case 0x0e:  interpret_xori(mips, memory);   return OLD_BLOCK;
    case 0x0f:  interpret_lui(mips, memory);    return OLD_BLOCK;
    case 0x20:  interpret_lb(mips, memory);     return OLD_BLOCK;
    case 0x21:  interpret_lh(mips, memory);     return OLD_BLOCK;
    case 0x22:  interpret_lwl(mips, memory);    return OLD_BLOCK;
    case 0x23:  interpret_lw(mips, memory);     return OLD_BLOCK;
    case 0x24:  interpret_lbu(mips, memory);    return OLD_BLOCK;
    case 0x25:  interpret_lhu(mips, memory);    return OLD_BLOCK;
    case 0x26:  interpret_lwr(mips, memory);    return OLD_BLOCK;
    case 0x28:  interpret_sb(mips, memory);     return OLD_BLOCK;
    case 0x29:  interpret_sh(mips, memory);     return OLD_BLOCK;
    case 0x2a:  interpret_swl(mips, memory);    return OLD_BLOCK;
    case 0x2b:  interpret_sw(mips, memory);     return OLD_BLOCK;
    case 0x2e:  interpret_swr(mips, memory);    return OLD_BLOCK;
    default:
        assert(0 && "Unknown instruction");
    };
secondary_op:
    switch((mips->r[MIPS_R_CIR] >> FN_SHIFT) & 
                                   FN_MASK) {
    case 0x00: interpret_sll(mips, memory);      return OLD_BLOCK;
    case 0x02: interpret_srl(mips, memory);      return OLD_BLOCK;
    case 0x03: interpret_sra(mips, memory);      return OLD_BLOCK;
    case 0x04: interpret_sllv(mips, memory);     return OLD_BLOCK;
    case 0x06: interpret_srlv(mips, memory);     return OLD_BLOCK;
    case 0x07: interpret_srav(mips, memory);     return OLD_BLOCK;
    case 0x08: interpret_jr(mips, memory);       return NEW_BLOCK;
    case 0x09: interpret_jalr(mips, memory);     return NEW_BLOCK;
    case 0x0c: interpret_syscall(mips, memory);  return OLD_BLOCK;
    case 0x0d: interpret_brk(mips, memory);      return OLD_BLOCK;
    case 0x10: interpret_mfhi(mips, memory);     return OLD_BLOCK;
    case 0x11: interpret_mthi(mips, memory);     return OLD_BLOCK;
    case 0x12: interpret_mflo(mips, memory);     return OLD_BLOCK;
    case 0x13: interpret_mtlo(mips, memory);     return OLD_BLOCK;
    case 0x18: interpret_mult(mips, memory);     return OLD_BLOCK;
    case 0x19: interpret_multu(mips, memory);    return OLD_BLOCK;
    case 0x1a: interpret_div(mips, memory);      return OLD_BLOCK;
    case 0x1b: interpret_divu(mips, memory);     return OLD_BLOCK;
    case 0x20: interpret_add(mips, memory);      return OLD_BLOCK;
    case 0x21: interpret_addu(mips, memory);     return OLD_BLOCK;
    case 0x22: interpret_sub(mips, memory);      return OLD_BLOCK;
    case 0x23: interpret_subu(mips, memory);     return OLD_BLOCK;
    case 0x24: interpret_and(mips, memory);      return OLD_BLOCK;
    case 0x25: interpret_or(mips, memory);       return OLD_BLOCK;
    case 0x26: interpret_xor(mips, memory);      return OLD_BLOCK;
    case 0x27: interpret_nor(mips, memory);      return OLD_BLOCK;
    case 0x2a: interpret_slt(mips, memory);      return OLD_BLOCK;
    case 0x2b: interpret_sltu(mips, memory);     return OLD_BLOCK;
    default:
        assert(0 && "Unknown instruction");
    };
branch_op:
    switch((mips->r[MIPS_R_CIR] >> RT_SHIFT) & 
                                   RT_MASK) {
    case 0x00: interpret_bltz(mips, memory);     return NEW_BLOCK;
    case 0x01: interpret_bgez(mips, memory);     return NEW_BLOCK;
    case 0x16: interpret_bltzal(mips, memory);   return NEW_BLOCK;
    case 0x17: interpret_bgezal(mips, memory);   return NEW_BLOCK;
    default:
        assert(0 && "Unknown instruction");
    };

    assert(0 && "Unknown instruction");
}

#define THRESHOLD 1000
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s mips.elf\n", *argv);
        return 1;
    }

    /* create the processor and memory */
    struct mips   mips   = {};
    struct memory memory = {};

    /* handle all explicit object creation */
    create_memory(&memory);

    /* load elf and set up stack */
    loader_elf(&mips, &memory, *++argv);

    /* create the jit runtime */
    asmjit::JitRuntime runtime;

    /* stores code blocks that have been *
     * jit compiled                      */
    std::map<u32, block> blocktable;

    /* stores addresses of jumps and branch    *
     * instructions and their execution counts */
    std::map<u32, u32> hotcount;

    /* main execution loop */
    for (;;) {
        if (interpret(&mips, &memory) == NEW_BLOCK) {
            /* if the current address is found in the *
             * block table execute the compiled block */
            if (auto b  = blocktable.find(mips.r[MIPS_R_PC]); 
                     b != blocktable.end()) {
                b->second(&mips, &memory); continue;
            }

            /* increment the hot count of the block, *
             * if there is no block of this address  *
             * add it to the map.                    */
            if (++hotcount[mips.r[MIPS_R_PC]] > THRESHOLD) {
                blocktable[mips.r[MIPS_R_PC]] = 
                    jitcompile(&runtime, &memory, mips.r[MIPS_R_PC]);
            }
        }
    }

    delete_memory(&memory);

    return 0;
}

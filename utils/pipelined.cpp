#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <pthread.h>

#include "log.h"
#include "mips.h"
#include "memory.h"
#include "icache.h"
#include "loader.h"

#include "interpreter_templates.h"

enum IType {
    ITYPE_BASIC         = 0,
    ITYPE_HALT          = 1,
    ITYPE_BRANCH        = 2,
    ITYPE_JUMP          = 4,
    ITYPE_JUMP_REG      = 8,
    ITYPE_JUMP_REG_LINK = 16,
    ITYPE_JUMP_LINK     = 32,
};

struct context {
    struct ICache *icache;
    struct mips   *mips;
    struct memory *memory;
};

IType decode(struct ICache *ic, u32 cir) {
    /* process opcode */
    switch((cir >> OP_SHIFT) & OP_MASK) {
    case 0x0: goto secondary_op;
    case 0x1: goto    branch_op;

    case 0x02: ICache_enqueue(ic, (void *) &interpret_j);       return ITYPE_JUMP;
    case 0x03: ICache_enqueue(ic, (void *) &interpret_jal);     return ITYPE_JUMP_LINK;
    case 0x04: ICache_enqueue(ic, (void *) &interpret_beq);     return ITYPE_BRANCH;
    case 0x05: ICache_enqueue(ic, (void *) &interpret_bne);     return ITYPE_BRANCH;
    case 0x06: ICache_enqueue(ic, (void *) &interpret_blez);    return ITYPE_BRANCH;
    case 0x07: ICache_enqueue(ic, (void *) &interpret_bgtz);    return ITYPE_BRANCH;

    case 0x08: ICache_enqueue(ic, (void *) &interpret_addi);    return ITYPE_BASIC;
    case 0x09: ICache_enqueue(ic, (void *) &interpret_addiu);   return ITYPE_BASIC;
    case 0x0a: ICache_enqueue(ic, (void *) &interpret_slti);    return ITYPE_BASIC;
    case 0x0b: ICache_enqueue(ic, (void *) &interpret_sltiu);   return ITYPE_BASIC;
    case 0x0c: ICache_enqueue(ic, (void *) &interpret_andi);    return ITYPE_BASIC;
    case 0x0d: ICache_enqueue(ic, (void *) &interpret_ori);     return ITYPE_BASIC;
    case 0x0e: ICache_enqueue(ic, (void *) &interpret_xori);    return ITYPE_BASIC;
    case 0x0f: ICache_enqueue(ic, (void *) &interpret_lui);     return ITYPE_BASIC;
    case 0x20: ICache_enqueue(ic, (void *) &interpret_lb);      return ITYPE_BASIC;
    case 0x21: ICache_enqueue(ic, (void *) &interpret_lh);      return ITYPE_BASIC;
    case 0x22: ICache_enqueue(ic, (void *) &interpret_lwl);     return ITYPE_BASIC;
    case 0x23: ICache_enqueue(ic, (void *) &interpret_lw);      return ITYPE_BASIC;
    case 0x24: ICache_enqueue(ic, (void *) &interpret_lbu);     return ITYPE_BASIC;
    case 0x25: ICache_enqueue(ic, (void *) &interpret_lhu);     return ITYPE_BASIC;
    case 0x26: ICache_enqueue(ic, (void *) &interpret_lwr);     return ITYPE_BASIC;
    case 0x28: ICache_enqueue(ic, (void *) &interpret_sb);      return ITYPE_BASIC;
    case 0x29: ICache_enqueue(ic, (void *) &interpret_sh);      return ITYPE_BASIC;
    case 0x2a: ICache_enqueue(ic, (void *) &interpret_swl);     return ITYPE_BASIC;
    case 0x2b: ICache_enqueue(ic, (void *) &interpret_sw);      return ITYPE_BASIC;
    case 0x2e: ICache_enqueue(ic, (void *) &interpret_swr);     return ITYPE_BASIC;
    default:
        assert(0 && "Unknown primary instruction");
    }
secondary_op:
    switch((cir >> FN_SHIFT) & FN_MASK) {
    case 0x0d: ICache_enqueue(ic, (void *) &interpret_brk);      return ITYPE_HALT;

    case 0x08: ICache_enqueue(ic, (void *) &interpret_jr);       return ITYPE_JUMP_REG;
    case 0x09: ICache_enqueue(ic, (void *) &interpret_jalr);     return ITYPE_JUMP_REG_LINK;

    case 0x00: ICache_enqueue(ic, (void *) &interpret_sll);      return ITYPE_BASIC;
    case 0x02: ICache_enqueue(ic, (void *) &interpret_srl);      return ITYPE_BASIC;
    case 0x03: ICache_enqueue(ic, (void *) &interpret_sra);      return ITYPE_BASIC;
    case 0x04: ICache_enqueue(ic, (void *) &interpret_sllv);     return ITYPE_BASIC;
    case 0x06: ICache_enqueue(ic, (void *) &interpret_srlv);     return ITYPE_BASIC;
    case 0x07: ICache_enqueue(ic, (void *) &interpret_srav);     return ITYPE_BASIC;
    case 0x0c: ICache_enqueue(ic, (void *) &interpret_syscall);  return ITYPE_BASIC;
    case 0x10: ICache_enqueue(ic, (void *) &interpret_mfhi);     return ITYPE_BASIC;
    case 0x11: ICache_enqueue(ic, (void *) &interpret_mthi);     return ITYPE_BASIC;
    case 0x12: ICache_enqueue(ic, (void *) &interpret_mflo);     return ITYPE_BASIC;
    case 0x13: ICache_enqueue(ic, (void *) &interpret_mtlo);     return ITYPE_BASIC;
    case 0x18: ICache_enqueue(ic, (void *) &interpret_mult);     return ITYPE_BASIC;
    case 0x19: ICache_enqueue(ic, (void *) &interpret_multu);    return ITYPE_BASIC;
    case 0x1a: ICache_enqueue(ic, (void *) &interpret_div);      return ITYPE_BASIC;
    case 0x1b: ICache_enqueue(ic, (void *) &interpret_divu);     return ITYPE_BASIC;
    case 0x20: ICache_enqueue(ic, (void *) &interpret_add);      return ITYPE_BASIC;
    case 0x21: ICache_enqueue(ic, (void *) &interpret_addu);     return ITYPE_BASIC;
    case 0x22: ICache_enqueue(ic, (void *) &interpret_sub);      return ITYPE_BASIC;
    case 0x23: ICache_enqueue(ic, (void *) &interpret_subu);     return ITYPE_BASIC;
    case 0x24: ICache_enqueue(ic, (void *) &interpret_and);      return ITYPE_BASIC;
    case 0x25: ICache_enqueue(ic, (void *) &interpret_or);       return ITYPE_BASIC;
    case 0x26: ICache_enqueue(ic, (void *) &interpret_xor);      return ITYPE_BASIC;
    case 0x27: ICache_enqueue(ic, (void *) &interpret_nor);      return ITYPE_BASIC;
    case 0x2a: ICache_enqueue(ic, (void *) &interpret_slt);      return ITYPE_BASIC;
    case 0x2b: ICache_enqueue(ic, (void *) &interpret_sltu);     return ITYPE_BASIC;
    default:
        assert(0 && "Unknown secondary instruction");
    }
branch_op:
    switch((cir >> RT_SHIFT) & RT_MASK) {
    case 0x00: ICache_equeue(ic, (void *) &interpret_bltz);     return ITYPE_BRANCH;
    case 0x01: ICache_equeue(ic, (void *) &interpret_bgez);     return ITYPE_BRANCH;
    case 0x10: ICache_equeue(ic, (void *) &interpret_bltzal);   return ITYPE_BRANCH;
    case 0x11: ICache_equeue(ic, (void *) &interpret_bgezal);   return ITYPE_BRANCH;
    default:
        assert(0 && "Unknown branch instruction");
    }
}

static inline bool is_halt(enum IType t) {
    return ((t & ITYPE_HALT) > 0);
}
static inline bool is_control_flow(enum IType t) {
    return ((t & (ITYPE_BRANCH|
                  ITYPE_JUMP|
                  ITYPE_JUMP_LINK|
                  ITYPE_JUMP_REG_LINK|
                  ITYPE_JUMP_REG)) > 0);
}
static void *decoder(void *args) {
    // retrive all paramters 
    struct ICache *ic     = (struct context *) args->icache;
    struct mips   *mips   = (struct context *) args->mips;
    struct memory *memory = (struct context *) args->memory;
    
    u32 shadow_pc = mips->r[MIPS_R_PC];

    for (IType type = 0; !is_halt(type); type=0) {
        // some sort of mechanism to 
        // check if work queue needs
        // more instructions
        // Can increase the threshold for 
        // bulk decoding
        while (ICache_length(ic) < PREFETCHER_SIZE)
            ;

        // some sort of pre-fetcher, allows the 
        // instructions from memory to be closer
        // to decoder in memory and reduce fetch
        // overhead
        u32 prefetcher[PREFETCHER_SIZE];
        
        // Could possibly create a separate thread
        // for fetching, it speculates N number of 
        // blocks ahead of the decoder.
        // Such a system would require speculative
        // branching and jumps
        memory_read(memory, shadow_pc, 
                    prefetcher, PREFETCHER_SIZE);
        
        // update the shadow pc to point to the
        // next block of instructions
        shadow_pc += PREFETCHER_SIZE;

        // decode instructions into all 
        // slots in icache before checking
        // halt hit
        u64 unused_slots = PREFETCHER_SIZE;
        while (unused_slots--) {
            // decode the current instruction
            // in the prefetcher and add it to 
            // ICache for execution
            type |= decode(ic, *prefetcher++);
            
            // might be an expensive if condition
            // maybe better to take this out of
            // the inner decode loop and check if
            // a control flow has been hit, then
            // altering the ICache based on that.
            // Might need to implement a "under-write"
            // signal within the ICache to signal that
            // instruction decode needs to change due
            // to control hazard
            if (is_control_flow(type)) {
                // decode following instruction
                // to complete branch delay
                type |= decode(ic, *prefetcher++);

                // push branch delay instruction
                ICache_enqueue(ic, (void *) &interpret_branch_delay);

                // if instruction is a branch
                // or a jump reg, jump link or 
                // jump link and reg wait until 
                // previous instructions have 
                // been processed
                while (ICache_length(ic) < ICACHE_SIZE)
                    ;
                
                // update the local pc to reflect new
                // real location
                shadow_pc = mips->r[MIPS_R_PC];

                // force to next decode loop
                break;
            }
        }
    }
    // add some sort of halt function, it should
    // emulate the ICache_enqueue of the branch
    // delay done whenever a control flow is detected
    // only it works for halting
    
    // wait until space in ICache for 
    // halt signal instruction
    while (ICache_length(ic) == 0)
        ;
    
    // send halt signal
    ICache_enqueue(ic, (void *) &interpret_signal_halt);

    return NULL;
}

static void *execute(void *args) {
    // retrive all paramters 
    struct ICache *ic     = (struct context *) args->icache;
    struct mips   *mips   = (struct context *) args->mips;
    struct memory *memory = (struct context *) args->memory;

    for (;;) {
        // some sort of mechanism to 
        // check if work queue needs
        // more instructions
        // Can increase the threshold for 
        // bulk execution 
        u64 filled_slots;
        while ((filled_slots = ICache_length(ic)) <= PREFETCH_SIZE/2)
            ;
        
        // execute decoded instructions
        while (filled_slots--)
            (block) ICache_dequeue(ic)(mips, memory);

        // some method to signal from decoder to
        // executor to exit once the brk instruction
        // has been encountered
        if (mips->halted)
            break;
    }

finished:
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s mips.elf\n", *argv);
        return 1;
    }

    struct mips   mips   = {};
    struct memory memory = {};

    struct ICache ICache = {};

    /* handle any explicit object creation */
    create_memory(&memory);
    
    /* load elf into memory */
    loader_elf(&mips, &memory, *++argv);
    
    // create thread arguments
    struct context ctx = {
        .icache = &ICache,
        .mips   = &mips,
        .memory = &memory
    };

    // define threads
    pthread_t thread_execute;
    pthread_t thread_decode;

    // spawn threads
    // create execute first since it will poll for
    // new instructions depending decode
    assert(!pthread_create(&thread_execute, NULL, execute, (void *) ctx));
    assert(!pthread_create(&thread_decode , NULL, decode , (void *) ctx));

    // join threads for exit
    pthread_join(thread_decode , NULL);
    pthread_join(thread_execute, NULL);

    /* clean up */
    delete_memory(&memory);

    return 0;
}

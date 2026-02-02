#ifndef __TEST_BRANCH_EQ_NE__
#define __TEST_BRANCH_EQ_NE__

#define TEST_ASSERT_BRANCH_EQ_NE(name)                                                          \
    TEST_ASSERT(mips.r[MIPS_R_PC] == expected, name,                                            \
            "%%pc=0x%08x", "%%pc=0x%08x when %%a0=0x%08x, %%a1=0x%08x, imm=0x%08x",             \
            expected, mips.r[MIPS_R_PC], mips.r[MIPS_R_A0], mips.r[MIPS_R_A1], JUMP_RELATIVE)

#define DEFINE_GOAL_FLAG(loc)                                \
    /* create goal segment */                                \
    struct segment *segment =                                \
        create_segment(&memory, (loc << 2) + TEST_START + 4, \
                                (loc << 2) + TEST_START + 8, \
                                256, 4);                     \
    assert(segment != NULL);                                 \
    /* place goal flag */                                    \
    *((u32 *) segment->segment) =                            \
        ASSEMBLE_HALT

/********************************************
 * BRANCH NE EQ: TODO: Design sequence
 * test will combine all instructions of this
 * family. Allows to test compatibility across
 * instructions and gives a general idea
 ********************************************/
#if 0
static TEST_FUNC(test_family_branch_ne_eq) {
    TEST_SEQUENCE(0,
        ASSEMBLE_HALT);

    TEST_EXECUTE;

    TEST_ASSERT();

    delete_memory(&memory);
}
#endif

#define JUMP_RELATIVE 2
/********************************************
 * BEQ
 ********************************************/
static TEST_FUNC(test_beq_before_delay) {
    u32 expected = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BEQ_OP, MIPS_R_A0, MIPS_R_A1, JUMP_RELATIVE),
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0xDEADBEEF;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("beq before load");

    delete_memory(&memory);
}
static TEST_FUNC(test_beq_after_delay) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BEQ_OP, MIPS_R_A0, MIPS_R_A1, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0xDEADBEEF;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("beq after load");

    delete_memory(&memory);
}
static TEST_FUNC(test_beq_jump_min) {
    u32 jumpto   = (s32) (s16) 0x8000;
    u32 expected = (jumpto << 2) + TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BEQ_OP, MIPS_R_A0, MIPS_R_A1, jumpto),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT);
    
    DEFINE_GOAL_FLAG(jumpto);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0xDEADBEEF;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("beq jump min");

    delete_memory(&memory);
}
static TEST_FUNC(test_beq_jump_max) {
    u32 jumpto   = (s32) (s16) 0x7fff;
    u32 expected = (jumpto << 2) + TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BEQ_OP, MIPS_R_A0, MIPS_R_A1, jumpto),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT);

    DEFINE_GOAL_FLAG(jumpto);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0xDEADBEEF;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("beq jump max");

    delete_memory(&memory);
}

/********************************************
 * BNE
 ********************************************/
static TEST_FUNC(test_bne_before_delay) {
    u32 expected = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BNE_OP, MIPS_R_A0, MIPS_R_A1, JUMP_RELATIVE),
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0x00000000;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("bne before load");

    delete_memory(&memory);
}
static TEST_FUNC(test_bne_after_delay) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BNE_OP, MIPS_R_A0, MIPS_R_A1, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0x00000000;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("bne after load");

    delete_memory(&memory);
}
static TEST_FUNC(test_bne_jump_min) {
    u32 jumpto   = (s32) (s16) 0x8000;
    u32 expected = (jumpto << 2) + TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BNE_OP, MIPS_R_A0, MIPS_R_A1, jumpto),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT);
    
    DEFINE_GOAL_FLAG(jumpto);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0x00000000;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("bne jump min");

    delete_memory(&memory);
}
static TEST_FUNC(test_bne_jump_max) {
    u32 jumpto   = (s32) (s16) 0x7fff;
    u32 expected = (jumpto << 2) + TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_EQ_NE(BNE_OP, MIPS_R_A0, MIPS_R_A1, jumpto),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT);
    
    DEFINE_GOAL_FLAG(jumpto);

    mips.r[MIPS_R_A0] = 0xDEADBEEF;
    mips.r[MIPS_R_A1] = 0x00000000;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_EQ_NE("bne jump max");

    delete_memory(&memory);
}

// clean up
#undef JUMP_RELATIVE
#undef DEFINE_GOAL_FLAG
#undef TEST_ASSERT_BRANCH_EQ_NE

#endif // __TEST_BRANCH_EQ_NE__

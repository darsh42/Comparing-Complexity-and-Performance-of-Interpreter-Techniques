#ifndef __TEST_BRANCH_GENERIC__
#define __TEST_BRANCH_GENERIC__

#define TEST_ASSERT_BRANCH_GENERIC(name)                                   \
    TEST_ASSERT(mips.r[MIPS_R_PC] == expected, name,                       \
            "%%pc=0x%08x", "%%pc=0x%08x when %%a0=0x%08x, imm=0x%08x",     \
            expected, mips.r[MIPS_R_PC], mips.r[MIPS_R_A0], JUMP_RELATIVE)

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
 * BRANCH GENERIC: TODO: Design sequence
 * test will combine all instructions of this
 * family. Allows to test compatibility across
 * instructions and gives a general idea
 ********************************************/
#if 0
static TEST_FUNC(test_family_branch_generic) {
    TEST_SEQUENCE(0,
        ASSEMBLE_HALT);

    TEST_EXECUTE;

    TEST_ASSERT();

    delete_memory(&memory);
}
#endif

#define JUMP_RELATIVE 2
/********************************************
 * BLTZ
 ********************************************/
static TEST_FUNC(test_bltz_compare_less) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BLTZ_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = -1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bltz compare less");

    delete_memory(&memory);
}
static TEST_FUNC(test_bltz_compare_equal) {
    u32 expected = TEST_START + 12;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BLTZ_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bltz compare equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_bltz_compare_more) {
    u32 expected = TEST_START + 12;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BLTZ_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bltz compare more");

    delete_memory(&memory);
}
/********************************************
 * BGEZ
 ********************************************/
static TEST_FUNC(test_bgez_compare_less) {
    u32 expected = TEST_START + 12;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BGEZ_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = -1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgez compare less");

    delete_memory(&memory);
}
static TEST_FUNC(test_bgez_compare_equal) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BGEZ_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgez compare equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_bgez_compare_more) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BGEZ_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgez compare more");

    delete_memory(&memory);
}
/********************************************
 * BLEZ
 ********************************************/
static TEST_FUNC(test_blez_compare_less) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_BLEZ_BGTZ(BLEZ_OP, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = -1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("blez compare less");

    delete_memory(&memory);
}
static TEST_FUNC(test_blez_compare_equal) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_BLEZ_BGTZ(BLEZ_OP, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("blez compare equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_blez_compare_more) {
    u32 expected = TEST_START + 12;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_BLEZ_BGTZ(BLEZ_OP, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("blez compare more");

    delete_memory(&memory);
}
/********************************************
 * BGTZ
 ********************************************/
static TEST_FUNC(test_bgtz_compare_less) {
    u32 expected = TEST_START + 12;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_BLEZ_BGTZ(BGTZ_OP, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = -1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgtz compare less");

    delete_memory(&memory);
}
static TEST_FUNC(test_bgtz_compare_equal) {
    u32 expected = TEST_START + 12;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_BLEZ_BGTZ(BGTZ_OP, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgtz compare equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_bgtz_compare_more) {
    u32 expected = TEST_START + 16;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_BLEZ_BGTZ(BGTZ_OP, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgtz compare more");

    delete_memory(&memory);
}

/* re-define assert for link register check */
#undef  TEST_ASSERT_BRANCH_GENERIC
#define TEST_ASSERT_BRANCH_GENERIC(name)                                                                 \
    TEST_ASSERT(mips.r[MIPS_R_PC] == expected_pc && mips.r[MIPS_R_RA] == expected_ra,                    \
            name, "%%pc=0x%08x %%ra=0x%08x", "%%pc=0x%08x %%ra=0x%08x when %%a0=0x%08x, imm=0x%08x",     \
            expected_pc, expected_ra, mips.r[MIPS_R_PC], mips.r[MIPS_R_RA], mips.r[MIPS_R_A0], JUMP_RELATIVE)
/********************************************
 * BLTZAL
 ********************************************/
static TEST_FUNC(test_bltzal_compare_less) {
    u32 expected_pc = TEST_START + 16;
    u32 expected_ra = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BLTZAL_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = -1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bltzal compare less");

    delete_memory(&memory);
}
static TEST_FUNC(test_bltzal_compare_equal) {
    u32 expected_pc = TEST_START + 12;
    u32 expected_ra = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BLTZAL_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bltzal compare equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_bltzal_compare_more) {
    u32 expected_pc = TEST_START + 12;
    u32 expected_ra = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BLTZAL_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bltzal compare more");

    delete_memory(&memory);
}
/********************************************
 * BGEZAL
 ********************************************/
static TEST_FUNC(test_bgezal_compare_less) {
    u32 expected_pc = TEST_START + 12;
    u32 expected_ra = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BGEZAL_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = -1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgezal compare less");

    delete_memory(&memory);
}
static TEST_FUNC(test_bgezal_compare_equal) {
    u32 expected_pc = TEST_START + 16;
    u32 expected_ra = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BGEZAL_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgezal compare equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_bgezal_compare_more) {
    u32 expected_pc = TEST_START + 16;
    u32 expected_ra = TEST_START + 8;

    TEST_SEQUENCE(
        ASSEMBLE_BRANCH_GENERIC(BGEZAL_RT, MIPS_R_A0, JUMP_RELATIVE),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT,
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;

    TEST_EXECUTE;
    
    TEST_ASSERT_BRANCH_GENERIC("bgezal compare more");

    delete_memory(&memory);
}

#undef JUMP_RELATIVE
#undef DEFINE_GOAL_FLAG
#undef TEST_ASSERT_BRANCH

#endif // __TEST_BRANCH_GENERIC__

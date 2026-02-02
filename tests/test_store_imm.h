#ifndef __TEST_STORE_IMM__
#define __TEST_STORE_IMM__

#define DATA 0xDEADBEEF
#define RAM_PERM  256
#define RAM_SIZE  4
#define RAM_START 20
#define RAM_END   RAM_START + RAM_SIZE
#define DEFINE_DATA_GOAL                   \
    /* create goal segment */              \
    struct segment *segment =              \
        create_segment(&memory, RAM_START, \
                                RAM_END,   \
                                RAM_PERM,  \
                                RAM_SIZE); \
    assert(segment != NULL);
#define TEST_ASSERT_STORE_IMM(name)                                                     \
    {                                                                                   \
        u32 read;                                                                       \
        memory_read(&memory, RAM_START, &read, 4);                                      \
        TEST_ASSERT(read == expected, name,                                             \
                "read=0x%08x", "read=0x%08x when %%at=0x%08x, %%a0=0x%08x, imm=0x%08x", \
                expected, read, mips.r[MIPS_R_AT], mips.r[MIPS_R_A0], immediate);       \
    }

TEST_FUNC(test_sb) {
    u32 expected  = DATA & 0xff;
    u32 immediate = RAM_START;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_STORE_IMM(SB_OP, MIPS_R_AT, MIPS_R_A0, immediate),
        ASSEMBLE_HALT);

    /* set data pattern to store into register*/
    mips.r[MIPS_R_AT] = DATA;

    /* allocate space in memory scheme */
    DEFINE_DATA_GOAL;

    /* execute sequence */
    TEST_EXECUTE;

    /* check memory location */
    TEST_ASSERT_STORE_IMM("test_sb");

    /* clean up */
    delete_memory(&memory);
}
TEST_FUNC(test_sh) {
    u32 expected  = DATA & 0xffff;
    u32 immediate = RAM_START;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_STORE_IMM(SH_OP, MIPS_R_AT, MIPS_R_A0, immediate),
        ASSEMBLE_HALT);

    /* set data pattern to store into register*/
    mips.r[MIPS_R_AT] = DATA;

    /* allocate space in memory scheme */
    DEFINE_DATA_GOAL;

    /* execute sequence */
    TEST_EXECUTE;

    /* check memory location */
    TEST_ASSERT_STORE_IMM("test_sh");

    /* clean up */
    delete_memory(&memory);
}
TEST_FUNC(test_sw) {
    u32 expected  = DATA;
    u32 immediate = RAM_START;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_STORE_IMM(SW_OP, MIPS_R_AT, MIPS_R_A0, immediate),
        ASSEMBLE_HALT);

    /* set data pattern to store into register*/
    mips.r[MIPS_R_AT] = DATA;

    /* allocate space in memory scheme */
    DEFINE_DATA_GOAL;

    /* execute sequence */
    TEST_EXECUTE;

    /* check memory location */
    TEST_ASSERT_STORE_IMM("test_sw");

    /* clean up */
    delete_memory(&memory);
}

#undef TEST_ASSERT_STORE_IMM
#undef DATA
#undef RAM_PERM
#undef RAM_SIZE
#undef RAM_START
#undef RAM_END

#endif // __TEST_STORE_IMM__

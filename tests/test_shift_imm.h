#ifndef __TEST_SHIFT_IMM__
#define __TEST_SHIFT_IMM__

#define TEST_ASSERT_SHIFT_IMM(name)                                    \
    TEST_ASSERT(mips.r[MIPS_R_AT] == expected, name,                   \
            "%%at=0x%08x", "%%at=0x%08x when %%a0=0x%08x, imm=0x%02x", \
            expected, mips.r[MIPS_R_AT], mips.r[MIPS_R_A0], immediate)

TEST_FUNC(test_sll) {
    u32 expected  = 0xDEAD0000;
    u32 immediate = 16;
    
    TEST_SEQUENCE(
        ASSEMBLE_SHIFT_IMM(SLL_FN, MIPS_R_AT, MIPS_R_A0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEAD;

    TEST_EXECUTE;

    TEST_ASSERT_SHIFT_IMM("sll");

    delete_memory(&memory);
}
TEST_FUNC(test_srl) {
    u32 expected  = 0xDEAD;
    u32 immediate = 16;

    TEST_SEQUENCE(
        ASSEMBLE_SHIFT_IMM(SRL_FN, MIPS_R_AT, MIPS_R_A0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEAD0000;

    TEST_EXECUTE;

    TEST_ASSERT_SHIFT_IMM("srl");

    delete_memory(&memory);
}
TEST_FUNC(test_sra) {
    u32 expected  = 0xFFFFDEAD;
    u32 immediate = 16;

    TEST_SEQUENCE(
        ASSEMBLE_SHIFT_IMM(SRA_FN, MIPS_R_AT, MIPS_R_A0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEAD0000;

    TEST_EXECUTE;

    TEST_ASSERT_SHIFT_IMM("sra");

    delete_memory(&memory);
}

#undef TEST_ASSERT_SHIFT_IMM

#endif // __TEST_SHIFT_IMM__

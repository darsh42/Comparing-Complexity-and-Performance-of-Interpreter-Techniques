#ifndef __TEST_SHIFT_REG__
#define __TEST_SHIFT_REG__

#define TEST_ASSERT_SHIFT_REG(name)                                     \
    TEST_ASSERT(mips.r[MIPS_R_AT] == expected, name,                    \
            "%%at=0x%08x", "%%at=0x%08x when %%a0=0x%08x, %%a1=0x%02x", \
            expected, mips.r[MIPS_R_AT], mips.r[MIPS_R_A0], mips.r[MIPS_R_A1])

TEST_FUNC(test_sllv) {
    u32 expected  = 0xDEAD0000;
    u32 immediate = 16;
    
    TEST_SEQUENCE(
        ASSEMBLE_SHIFT_REG(SLLV_FN, MIPS_R_AT, MIPS_R_A0, MIPS_R_A1),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEAD;
    mips.r[MIPS_R_A1] = 16;

    TEST_EXECUTE;

    TEST_ASSERT_SHIFT_REG("sll");

    delete_memory(&memory);
}
TEST_FUNC(test_srlv) {
    u32 expected  = 0xDEAD;

    TEST_SEQUENCE(
        ASSEMBLE_SHIFT_REG(SRLV_FN, MIPS_R_AT, MIPS_R_A0, MIPS_R_A1),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEAD0000;
    mips.r[MIPS_R_A1] = 16;

    TEST_EXECUTE;

    TEST_ASSERT_SHIFT_REG("srl");

    delete_memory(&memory);
}
TEST_FUNC(test_srav) {
    u32 expected  = 0xFFFFDEAD;

    TEST_SEQUENCE(
        ASSEMBLE_SHIFT_REG(SRAV_FN, MIPS_R_AT, MIPS_R_A0, MIPS_R_A1),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0xDEAD0000;
    mips.r[MIPS_R_A1] = 16;

    TEST_EXECUTE;

    TEST_ASSERT_SHIFT_REG("sra");

    delete_memory(&memory);
}

#undef TEST_ASSERT_SHIFT_REG

#endif // __TEST_SHIFT_REG__


#ifndef __TEST_MF__
#define __TEST_MF__

#define DATA 0xDEADBEEF
#define TEST_ASSERT_MFHI(name)                                  \
    TEST_ASSERT(mips.r[MIPS_R_AT] == expected, name,            \
            "%%at=0x%08x", "%%at=0x%08x when %%hi=0x%08x",      \
            expected, mips.r[MIPS_R_AT], mips.r[MIPS_R_HI]);
#define TEST_ASSERT_MFLO(name)                                  \
    TEST_ASSERT(mips.r[MIPS_R_AT] == expected, name,            \
            "%%at=0x%08x", "%%at=0x%08x when %%lo=0x%08x",      \
            expected, mips.r[MIPS_R_AT], mips.r[MIPS_R_LO]);

TEST_FUNC(test_mfhi) {
    u32 expected = DATA;

    TEST_SEQUENCE(
        ASSEMBLE_MF(MFHI_FN, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_HI] = DATA;

    TEST_EXECUTE;

    TEST_ASSERT_MFHI("mfhi");

    delete_memory(&memory);
}

TEST_FUNC(test_mflo) {
    u32 expected = DATA;

    TEST_SEQUENCE(
        ASSEMBLE_MF(MFLO_FN, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_LO] = DATA;

    TEST_EXECUTE;

    TEST_ASSERT_MFLO("mflo");

    delete_memory(&memory);
}

#undef TEST_ASSERT_MFLO
#undef TEST_ASSERT_MFHI
#undef DATA

#endif // __TEST_MF__

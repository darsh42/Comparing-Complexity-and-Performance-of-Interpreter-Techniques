#ifndef __TEST_MT__
#define __TEST_MT__

#define DATA 0xDEADBEEF
#define TEST_ASSERT_MTHI(name)                                  \
    TEST_ASSERT(mips.r[MIPS_R_HI] == expected, name,            \
            "%%hi=0x%08x", "%%hi=0x%08x when %%at=0x%08x",      \
            expected, mips.r[MIPS_R_HI], mips.r[MIPS_R_AT]);
#define TEST_ASSERT_MTLO(name)                                  \
    TEST_ASSERT(mips.r[MIPS_R_LO] == expected, name,            \
            "%%lo=0x%08x", "%%lo=0x%08x when %%at=0x%08x",      \
            expected, mips.r[MIPS_R_LO], mips.r[MIPS_R_AT]);

TEST_FUNC(test_mthi) {
    u32 expected = DATA;

    TEST_SEQUENCE(
        ASSEMBLE_MT(MTHI_FN, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = DATA;

    TEST_EXECUTE;

    TEST_ASSERT_MTHI("mthi");

    delete_memory(&memory);
}

TEST_FUNC(test_mtlo) {
    u32 expected = DATA;

    TEST_SEQUENCE(
        ASSEMBLE_MT(MTLO_FN, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = DATA;

    TEST_EXECUTE;

    TEST_ASSERT_MTLO("mtlo");

    delete_memory(&memory);
}

#undef TEST_ASSERT_MTLO
#undef TEST_ASSERT_MTHI
#undef DATA

#endif // __TEST_MT__


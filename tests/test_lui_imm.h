#ifndef __TEST_LUI_IMM__
#define __TEST_LUI_IMM__

#define TEST_ASSERT_LUI_IMM(name)                         \
    TEST_ASSERT(mips.r[MIPS_R_AT] == expected, name,      \
            "%%at=0x%08x", "%%at=0x%08x when imm=0x%04x", \
            expected, mips.r[MIPS_R_AT], immediate)

static TEST_FUNC(test_lui_empty) {
    u32 expected  = 0xffff0000;
    u32 immediate = 0xffff;

    TEST_SEQUENCE(
        ASSEMBLE_LUI_IMM(LUI_OP, MIPS_R_AT, immediate),
        ASSEMBLE_HALT);

    TEST_EXECUTE;

    TEST_ASSERT_LUI_IMM("lui clear register");

    delete_memory(&memory);
}

static TEST_FUNC(test_lui_filled) {
    u32 expected  = 0xffff0000;
    u32 immediate = 0xffff;

    TEST_SEQUENCE(
        ASSEMBLE_LUI_IMM(LUI_OP, MIPS_R_AT, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0x12345678;

    TEST_EXECUTE;

    TEST_ASSERT_LUI_IMM("lui filled register");

    delete_memory(&memory);
}

#undef TEST_ASSERT_LOAD_IMM

#endif // __TEST_LUI_IMM__

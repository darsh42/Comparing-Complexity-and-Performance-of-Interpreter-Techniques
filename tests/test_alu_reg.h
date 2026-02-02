#ifndef __TEST_ALU_REG_H__
#define __TEST_ALU_REG_H__

#define TEST_ASSERT_ALU_REG(name)                                       \
    TEST_ASSERT(mips.r[MIPS_R_AT] == expected, name,                    \
            "%%at=0x%08x", "%%at=0x%08x when %%a0=0x%08x, %%a1=0x%08x", \
            expected, mips.r[MIPS_R_AT], mips.r[MIPS_R_A0], mips.r[MIPS_R_A1])

/********************************************
 * ALU REG: TODO: Design sequence
 * test will combine all instructions of this
 * family. Allows to test compatibility across
 * instructions and gives a general idea
 ********************************************/
#if 0
static TEST_FUNC(test_family_alu_reg) {
    TEST_SEQUENCE(0,
        ASSEMBLE_HALT);

    TEST_EXECUTE;

    TEST_ASSERT();

    delete_memory(&memory);
}
#endif

/********************************************
 * ADD 
 ********************************************/
static TEST_FUNC(test_add) {
    u32 expected = 2;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(ADD_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("add");

    delete_memory(&memory);
}
static TEST_FUNC(test_add_overflow) {
    u32 expected = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(ADD_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = U32MAX;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("add overflow");

    delete_memory(&memory);
}

/********************************************
 * ADDU 
 ********************************************/
static TEST_FUNC(test_addu) {
    u32 expected = 2;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(ADDU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 1;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("addu");

    delete_memory(&memory);
}
static TEST_FUNC(test_addu_overflow) {
    u32 expected = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(ADDU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = U32MAX;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("addu overflow");

    delete_memory(&memory);
}


/********************************************
 * SUB TODO: Fix under/over flow 
 ********************************************/
static TEST_FUNC(test_sub) {
    u32 expected = -1;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SUB_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("sub");

    delete_memory(&memory);
}
static TEST_FUNC(test_sub_overflow) {
    u32 expected = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SUB_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = S32MIN;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("sub overflow");

    delete_memory(&memory);
}

/********************************************
 * SUBU
 ********************************************/
static TEST_FUNC(test_subu_a) {
    u32 expected = -1;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SUBU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = 0;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("subu a");

    delete_memory(&memory);
}
static TEST_FUNC(test_subu_b) {
    u32 expected = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SUBU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_A0] = U32MAX;
    mips.r[MIPS_R_A1] = U32MAX;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("subu_b");

    delete_memory(&memory);
}

/********************************************
 * AND
 ********************************************/
static TEST_FUNC(test_and) {
    u32 expected  = 0xDEADBEEF;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(AND_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0xFFFFFFFF;
    mips.r[MIPS_R_A1] = 0xDEADBEEF;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("andi");

    delete_memory(&memory);
}

/********************************************
 * OR
 ********************************************/
static TEST_FUNC(test_or) {
    u32 expected  = 0xDEADBEEF;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(OR_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0xDEAD0000;
    mips.r[MIPS_R_A1] = 0x0000BEEF;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("or");

    delete_memory(&memory);
}

/********************************************
 * XOR
 ********************************************/
static TEST_FUNC(test_xor) {
    u32 expected  = 0xF0F00F0F;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(XOR_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0xF0F0F0F0;
    mips.r[MIPS_R_A1] = 0x0000FFFF;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("xor");

    delete_memory(&memory);
}

/********************************************
 * NOR 
 ********************************************/
static TEST_FUNC(test_nor) {
    u32 expected  = 0x0F0F0000;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(NOR_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0xF0F0F0F0;
    mips.r[MIPS_R_A1] = 0x0000FFFF;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("nor");

    delete_memory(&memory);
}

/********************************************
 * SLT 
 ********************************************/
static TEST_FUNC(test_slt_less) {
    u32 expected  = 1;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLT_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("slt less");

    delete_memory(&memory);
}
static TEST_FUNC(test_slt_more) {
    u32 expected  = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLT_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 1;
    mips.r[MIPS_R_A1] = 0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("slt more");

    delete_memory(&memory);
}
static TEST_FUNC(test_slt_equal) {
    u32 expected  = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLT_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0;
    mips.r[MIPS_R_A1] = 0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("slt equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_slt_negetive) {
    u32 expected  = 1;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLT_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = -1;
    mips.r[MIPS_R_A1] =  0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("slt negetive");

    delete_memory(&memory);
}
/********************************************
 * SLTU
 ********************************************/
static TEST_FUNC(test_sltu_less) {
    u32 expected  = 1;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLTU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0;
    mips.r[MIPS_R_A1] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("sltu less");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltu_more) {
    u32 expected  = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLTU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 1;
    mips.r[MIPS_R_A1] = 0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("sltu more");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltu_equal) {
    u32 expected  = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLTU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = 0;
    mips.r[MIPS_R_A1] = 0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("sltu equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltu_negetive) {
    u32 expected  = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_REG(SLTU_FN, MIPS_R_A0, MIPS_R_A1, MIPS_R_AT),
        ASSEMBLE_HALT
    );

    mips.r[MIPS_R_A0] = -1;
    mips.r[MIPS_R_A1] =  0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_REG("sltu negetive");

    delete_memory(&memory);
}

#undef TEST_ASSERT_ALU_REG

#endif // __TEST_ALU_REG_H__

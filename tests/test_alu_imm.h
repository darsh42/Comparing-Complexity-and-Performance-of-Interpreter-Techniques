#ifndef __TEST_ALU_IMM_H__
#define __TEST_ALU_IMM_H__

/* helper for generic instruction tests */
#define TEST_ASSERT_ALU_IMM(name)                                   \
    TEST_ASSERT(mips.r[MIPS_R_V0] == expected, name,                \
        "%%v0=0x%08x", "%%v0=0x%08x when %%at=0x%08x, imm=0x%08x",  \
        expected, mips.r[MIPS_R_V0], mips.r[MIPS_R_AT], immediate)


/********************************************
 * ALU IMM: TODO: Design sequence
 * test will combine all instructions of this
 * family. Allows to test compatibility across
 * instructions and gives a general idea
 ********************************************/
#if 0
static TEST_FUNC(test_family_alu_imm) {
    TEST_SEQUENCE(
        ASSEMBLE_HALT);

    TEST_EXECUTE;

    TEST_ASSERT();

    delete_memory(&memory);
}
#endif

/********************************************
 * ADDI
 ********************************************/
static TEST_FUNC(test_addi_positive) {
    u32 expected  = 2;
    u32 immediate = 1;

    /* create and load instruction */
    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ADDI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    /* load input values */
    mips.r[MIPS_R_AT] = 1;              
    mips.r[MIPS_R_V0] = 0;

    /* call implementation */
    TEST_EXECUTE;

    /* check condition */
    TEST_ASSERT_ALU_IMM("addi positive");

    delete_memory(&memory);
}

static TEST_FUNC(test_addi_negetive) {
    u32 expected  = -2;
    u32 immediate = -1; 

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ADDI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    /* load input values */
    mips.r[MIPS_R_V0] =  0;
    mips.r[MIPS_R_AT] = -1;

    /* call implementation */
    TEST_EXECUTE;

    /* check condition */
    TEST_ASSERT_ALU_IMM("addi negetive");

    delete_memory(&memory);
}

static TEST_FUNC(test_addi_overflow) {
    u32 expected  = 0;
    u32 immediate = 1;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ADDI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    /* load input values */
    mips.r[MIPS_R_AT] = 0xffffffff;

    /* call implementation */
    TEST_EXECUTE;

    /* check condition */
    TEST_ASSERT_ALU_IMM("addi overflow");

    delete_memory(&memory);
}

/********************************************
 * ADDIU
 ********************************************/
static TEST_FUNC(test_addiu_positive) {
    u32 expected  = 2;
    u32 immediate = 1;
    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 1;
    mips.r[MIPS_R_V0] = 0;

    /* call implementation */
    TEST_EXECUTE;
    
    TEST_ASSERT_ALU_IMM("addiu positive");

    delete_memory(&memory);
}
static TEST_FUNC(test_addiu_negetive) {
    u32 expected  = 0xffff8001;
    u32 immediate = 0x00008000;
    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 1;
    mips.r[MIPS_R_V0] = 0;

    /* call implementation */
    TEST_EXECUTE;
    
    TEST_ASSERT_ALU_IMM("addiu negetive");

    delete_memory(&memory);
}

static TEST_FUNC(test_addiu_overflow) {
    u32 expected  = 0x0000;
    u32 immediate = 0xffff;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 1;
    mips.r[MIPS_R_V0] = 0;

    /* call implementation */
    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("addiu overflow");

    delete_memory(&memory);
}

/********************************************
 * SLTI 
 ********************************************/
static TEST_FUNC(test_slti_less) {
    u32 expected  = 1;
    u32 immediate = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = (u32) S32MIN;
    mips.r[MIPS_R_V0] = 0;

    /* call implementation */
    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("slti less");

    delete_memory(&memory);
}
static TEST_FUNC(test_slti_more) {
    u32 expected  = 0;
    u32 immediate = 0x8000;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = U32MAX;
    mips.r[MIPS_R_V0] = 0;

    /* call implementation */
    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("slti more");

    delete_memory(&memory);
}
static TEST_FUNC(test_slti_equal) {
    u32 expected  = 0;
    u32 immediate = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0;

    /* call implementation */
    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("slti equal");

    delete_memory(&memory);
}

/********************************************
 * SLTIU
 ********************************************/
static TEST_FUNC(test_sltiu_less) {
    u32 expected  = 1;
    u32 immediate = 1;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("sltiu less");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltiu_more) {
    u32 expected  = 0;
    u32 immediate = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 1;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("sltiu more");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltiu_equal) {
    u32 expected  = 0;
    u32 immediate = 0;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("sltiu equal");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltiu_bit_extension_less) {
    u32 expected  = 1;
    u32 immediate = 0x8000;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0x8000;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("sltiu bit extension less");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltiu_bit_extension_more) {
    u32 expected  = 0;
    u32 immediate = 0x8000;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0xffff8001;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("sltiu bit extension more");

    delete_memory(&memory);
}
static TEST_FUNC(test_sltiu_bit_extension_equal) {
    u32 expected  = 0;
    u32 immediate = 0x8000;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(SLTIU_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0xffff8000;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("sltiu bit extension equal");

    delete_memory(&memory);
}

/********************************************
 * ANDI
 ********************************************/
static TEST_FUNC(test_andi) {
    u32 expected  = 0xBEEF;
    u32 immediate = 0xFFFF;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ANDI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0xDEADBEEF;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("andi");

    delete_memory(&memory);
}

/********************************************
 * ORI
 ********************************************/
static TEST_FUNC(test_ori) {
    u32 expected  = 0xDEADBEEF;
    u32 immediate = 0xBEEF;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(ORI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0xDEAD0000;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("ori");

    delete_memory(&memory);
}

/********************************************
 * XORI
 ********************************************/
static TEST_FUNC(test_xori) {
    u32 expected  = 0xF00F;
    u32 immediate = 0x00FF;

    TEST_SEQUENCE(
        ASSEMBLE_ALU_IMM(XORI_OP, MIPS_R_AT, MIPS_R_V0, immediate),
        ASSEMBLE_HALT);

    mips.r[MIPS_R_AT] = 0xF0F0;

    TEST_EXECUTE;

    TEST_ASSERT_ALU_IMM("xori");

    delete_memory(&memory);
}

#undef TEST_ASSERT_ALU_IMM

#endif // __TEST_ALU_IMM_H__

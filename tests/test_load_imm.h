#ifndef __TEST_LOAD_IMM__
#define __TEST_LOAD_IMM__

#define DATA 0xDEADBEEF
#define RAM_PERM  256
#define RAM_SIZE  4
#define RAM_START 12
#define RAM_END   RAM_START + RAM_SIZE
#define TEST_ASSERT_LOAD_IMM(name)                                  \
    TEST_ASSERT(mips.r[MIPS_R_AT] == expected, name,                \
            "%%at=0x%08x", "%%at=0x%08x when %%a0=0x%08x, imm=0x0", \
            expected, mips.r[MIPS_R_AT], mips.r[MIPS_R_A0])
#define DEFINE_DATA_FLAG                   \
    /* create goal segment */              \
    struct segment *segment =              \
        create_segment(&memory, RAM_START, \
                                RAM_END,   \
                                RAM_PERM,  \
                                RAM_SIZE); \
    assert(segment != NULL);               \
    /* place goal flag */                  \
    *((u32 *) segment->segment) =          \
        DATA
/********************************************
 * LOAD IMM: TODO: Design sequence
 * test will combine all instructions of this
 * family. Allows to test compatibility across
 * instructions and gives a general idea
 ********************************************/
#if 0
static TEST_FUNC(test_family_alu_imm) {
    TEST_SEQUENCE(0,
        ASSEMBLE_HALT);

    TEST_EXECUTE;

    TEST_ASSERT();

    delete_memory(&memory);
}
#endif

/********************************************
 * LB
 ********************************************/
static TEST_FUNC(test_lb_before_delay) {
    u32 expected = 0;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LB_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_HALT);
    
    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;
    
    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lb before delay");

    /* clean up */
    delete_memory(&memory);
}
static TEST_FUNC(test_lb_after_delay) {
    u32 expected = (u32) (s32) (s8) DATA;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LB_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_NOP, /* branch delay */
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lb after delay");

    /* clean up */
    delete_memory(&memory);
}
/********************************************
 * LH
 ********************************************/
static TEST_FUNC(test_lh_before_delay) {
    u32 expected = 0;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LH_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lh before delay");

    /* clean up */
    delete_memory(&memory);
}
static TEST_FUNC(test_lh_after_delay) {
    u32 expected = (u32) (s32) (s16) DATA;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LH_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_NOP, /* branch delay */
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lh after delay");

    /* clean up */
    delete_memory(&memory);
}

/********************************************
 * LW
 ********************************************/
static TEST_FUNC(test_lw_before_delay) {
    u32 expected = 0;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LW_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lw before delay");

    /* clean up */
    delete_memory(&memory);
}
static TEST_FUNC(test_lw_after_delay) {
    u32 expected = DATA;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LW_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_NOP, /* branch delay */
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lw after delay");

    /* clean up */
    delete_memory(&memory);
}

/********************************************
 * LBU
 ********************************************/
static TEST_FUNC(test_lbu_before_delay) {
    u32 expected = 0;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LBU_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lbu before delay");

    /* clean up */
    delete_memory(&memory);
}
static TEST_FUNC(test_lbu_after_delay) {
    u32 expected = (u32) (u8) DATA;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LBU_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_NOP, /* branch delay */
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lbu after delay");

    /* clean up */
    delete_memory(&memory);
}
/********************************************
 * LHU
 ********************************************/
static TEST_FUNC(test_lhu_before_delay) {
    u32 expected = 0;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LHU_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lhu before delay");

    /* clean up */
    delete_memory(&memory);
}
static TEST_FUNC(test_lhu_after_delay) {
    u32 expected = (u32) (u16) DATA;
    /* create sequence */
    TEST_SEQUENCE(
        ASSEMBLE_LOAD_IMM(LHU_OP, MIPS_R_A0, MIPS_R_AT, 0),
        ASSEMBLE_NOP, /* branch delay */
        ASSEMBLE_HALT);

    /* set the offset */
    mips.r[MIPS_R_A0] = RAM_START;

    /* create the data flag */
    DEFINE_DATA_FLAG;

    /* execute the sequence */
    TEST_EXECUTE;

    /* check registers */
    TEST_ASSERT_LOAD_IMM("lhu after delay");

    /* clean up */
    delete_memory(&memory);
}

/* TODO: LWL and LWR */

#undef DATA
#undef RAM_PERM
#undef RAM_SIZE
#undef RAM_START
#undef RAM_END

#endif // __TEST_LOAD_IMM__

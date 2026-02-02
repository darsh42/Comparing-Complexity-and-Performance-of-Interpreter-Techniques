#ifndef __TEST_J_JAL__
#define __TEST_J_JAL__

#define JUMP_MAX 0x3ffffff
#define JUMP_MIN 0x0000000
#define TEST_ASSERT_J_JAL(name)                           \
    TEST_ASSERT(mips.r[MIPS_R_PC] == expected, name,      \
            "%%pc=0x%08x", "%%pc=0x%08x when imm=0x%08x", \
            expected, mips.r[MIPS_R_PC], JUMP_MAX)
#define DEFINE_GOAL_FLAG(loc)                                               \
    /* create goal segment */                                               \
    struct segment *segment =                                               \
        create_segment(&memory, (TEST_START & 0xf0000000) + (loc << 2),     \
                                (TEST_START & 0xf0000000) + (loc << 2) + 4, \
                                256, 4);                                    \
    assert(segment != NULL);                                                \
    /* place goal flag */                                                   \
    *((u32 *) segment->segment) =                                           \
        ASSEMBLE_HALT

/********************************************
 * J JAL: TODO: Design sequence
 * test will combine all instructions of this
 * family. Allows to test compatibility across
 * instructions and gives a general idea
 ********************************************/
#if 0
static TEST_FUNC(test_family_j_jal) {
    TEST_SEQUENCE(
        ASSEMBLE_HALT);

    TEST_EXECUTE;

    TEST_ASSERT();

    delete_memory(&memory);
}
#endif

/********************************************
 * J
 ********************************************/
static TEST_FUNC(test_j_address_max) {
    u32 expected = (TEST_START & 0xf0000000) + (JUMP_MAX << 2) + 4;

    TEST_SEQUENCE(
        ASSEMBLE_J_JAL(J_OP, JUMP_MAX),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT);

    DEFINE_GOAL_FLAG(JUMP_MAX);

    TEST_EXECUTE;

    TEST_ASSERT_J_JAL("j address max");

    delete_memory(&memory);
}
static TEST_FUNC(test_j_address_min) {
    u32 expected = (TEST_START & 0xf0000000) + (JUMP_MIN << 2) + 4;

    TEST_SEQUENCE(
        ASSEMBLE_J_JAL(J_OP, JUMP_MIN),
        ASSEMBLE_NOP,
        ASSEMBLE_HALT);

    DEFINE_GOAL_FLAG(JUMP_MIN);

    TEST_EXECUTE;

    TEST_ASSERT_J_JAL("j address min");

    delete_memory(&memory);
}


#undef  TEST_ASSERT_J_JAL
#define TEST_ASSERT_J_JAL(name)                                                         \
    TEST_ASSERT(mips.r[MIPS_R_PC] == expected_pc && mips.r[MIPS_R_RA] == expected_ra,   \
            name, "%%pc=0x%08x %%ra=0x%08x", "%%pc=0x%08x %%ra=0x%08x when imm=0x%08x", \
            expected_pc, expected_ra, mips.r[MIPS_R_PC], mips.r[MIPS_R_RA], JUMP_MAX)
/********************************************
 * JAL
 ********************************************/
static TEST_FUNC(test_jal) {
   u32 expected_pc = (TEST_START & 0xf0000000) + (JUMP_MAX << 2) + 4;
   u32 expected_ra =  TEST_START + 8;
                                                                        
   TEST_SEQUENCE(
       ASSEMBLE_J_JAL(JAL_OP, JUMP_MAX),
       ASSEMBLE_NOP,
       ASSEMBLE_HALT);
                                                                        
   DEFINE_GOAL_FLAG(JUMP_MAX);
                                                                        
   TEST_EXECUTE;
                                                                        
   TEST_ASSERT_J_JAL("jal");
                                                                        
   delete_memory(&memory);
}
static TEST_FUNC(test_jal_ra_modification) {
   u32 expected_pc = (TEST_START & 0xf0000000) + (JUMP_MAX << 2) + 4;
   u32 expected_ra =  TEST_START + 12;
                                                                        
   TEST_SEQUENCE(
       ASSEMBLE_J_JAL(JAL_OP, JUMP_MAX),
       ASSEMBLE_ALU_IMM(ADDI_OP, MIPS_R_RA, MIPS_R_RA, 4),
       ASSEMBLE_HALT);

   DEFINE_GOAL_FLAG(JUMP_MAX);
                                                                        
   TEST_EXECUTE;
                                                                        
   TEST_ASSERT_J_JAL("jal ra modification");
                                                                        
   delete_memory(&memory);
}


#undef JUMP_MIN
#undef JUMP_MAX
#undef DEFINE_GOAL_FLAG
#undef TEST_ASSERT_J_JAL

#endif // __TEST_J_JAL__

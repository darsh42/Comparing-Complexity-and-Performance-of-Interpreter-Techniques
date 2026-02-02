#ifndef __TEST_JR__
#define __TEST_JR__

#define JUMP_MIN 0x00000000
#define JUMP_MAX 0xfffffff8

#define TEST_ASSERT_JR(name)                                 \
    TEST_ASSERT(mips.r[MIPS_R_PC] == expected_pc ,           \
            name, "%%pc=0x%08x", "%%pc=0x%08x %%a0=0x%08x",  \
            expected_pc, mips.r[MIPS_R_PC], mips.r[MIPS_R_A0])

#define DEFINE_GOAL_FLAG(loc)            \
    /* create goal segment */            \
    struct segment *segment =            \
        create_segment(&memory, loc,     \
                                loc + 4, \
                                256, 4); \
    assert(segment != NULL);             \
    /* place goal flag */                \
    *((u32 *) segment->segment) =        \
        ASSEMBLE_HALT

/********************************************
 * JR
 ********************************************/
static TEST_FUNC(test_jr_before_delay) {
   u32 expected_pc   = TEST_START + 8;
                                                                        
   TEST_SEQUENCE(
       ASSEMBLE_JR(JR_FN, MIPS_R_A0),
       ASSEMBLE_HALT);

   mips.r[MIPS_R_A0] = JUMP_MAX;

   DEFINE_GOAL_FLAG(JUMP_MAX);
                                                                        
   TEST_EXECUTE;
                                                                        
   TEST_ASSERT_JR("jr before delay");

   delete_memory(&memory);
}
static TEST_FUNC(test_jr_after_delay) {
   u32 expected_pc   = JUMP_MAX + 4;

   TEST_SEQUENCE(
       ASSEMBLE_JR(JR_FN, MIPS_R_A0),
       ASSEMBLE_NOP,
       ASSEMBLE_HALT);

   mips.r[MIPS_R_A0] = JUMP_MAX;

   DEFINE_GOAL_FLAG(JUMP_MAX);
                                                                        
   TEST_EXECUTE;
                                                                        
   TEST_ASSERT_JR("jr after delay");
                                                                        
   delete_memory(&memory);
}

#undef JUMP_MIN
#undef JUMP_MAX
#undef TEST_ASSERT_JR
#undef DEFINE_GOAL_FLAG

#endif // __TEST_JR__


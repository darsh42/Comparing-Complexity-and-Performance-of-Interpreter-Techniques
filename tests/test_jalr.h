#ifndef __TEST_JALR__
#define __TEST_JALR__

#define JUMP_MIN 0x00000000
#define JUMP_MAX 0xfffffff8

#define TEST_ASSERT_JALR(name)                                                          \
    TEST_ASSERT(mips.r[MIPS_R_PC] == expected_pc && mips.r[MIPS_R_A1] == expected_link, \
            name, "%%pc=0x%08x %%a1=0x%08x", "%%pc=0x%08x %%a0=0x%08x %%a1=0x%08x",     \
            expected_pc, expected_link, mips.r[MIPS_R_PC], mips.r[MIPS_R_A0], mips.r[MIPS_R_A1])

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
 * JALR
 ********************************************/
static TEST_FUNC(test_jalr) {
   u32 expected_pc   = JUMP_MAX + 4;
   u32 expected_link = TEST_START + 8;
                                                                        
   TEST_SEQUENCE(
       ASSEMBLE_JALR(JALR_FN, MIPS_R_A0, MIPS_R_A1),
       ASSEMBLE_NOP,
       ASSEMBLE_HALT);

   mips.r[MIPS_R_A0] = JUMP_MAX;
                                                                    
   DEFINE_GOAL_FLAG(JUMP_MAX);
                                                                        
   TEST_EXECUTE;
                                                                        
   TEST_ASSERT_JALR("jalr");
                                                                        
   delete_memory(&memory);
}
static TEST_FUNC(test_jalr_link_modification) {
   u32 expected_pc   = JUMP_MAX + 4;
   u32 expected_link = TEST_START + 12;
                                                                        
   TEST_SEQUENCE(
       ASSEMBLE_JALR(JALR_FN, MIPS_R_A0, MIPS_R_A1),
       ASSEMBLE_ALU_IMM(ADDI_OP, MIPS_R_A1, MIPS_R_A1, 4),
       ASSEMBLE_HALT);

   mips.r[MIPS_R_A0] = JUMP_MAX;

   DEFINE_GOAL_FLAG(JUMP_MAX);
                                                                        
   TEST_EXECUTE;
                                                                        
   TEST_ASSERT_JALR("jalr link register modification");

   delete_memory(&memory);
}

#undef JUMP_MIN
#undef JUMP_MAX
#undef TEST_ASSERT_JALR
#undef DEFINE_GOAL_FLAG

#endif // __TEST_JALR__

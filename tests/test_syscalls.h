#ifndef __TEST_SYSCALLS__
#define __TEST_SYSCALLS__

#define PROGRAM_START TEST_START
#define PROGRAM_SIZE  256
#define PROGRAM_PERM  256

#define RAM_START 0x1000
#define RAM_PERM  256

#define FILENAME "file.txt"
#define FILEFLAGS O_CREAT | O_RDWR
#define FILENAME_LENGTH  filename_len
#define FILENAME_POINTER RAM_START
#define  MESSAGE_POINTER (FILENAME_POINTER + FILENAME_LENGTH)
#define  MESSAGE_LENGTH  message_len
#define  MESSAGE "Hello World!\n"

static TEST_FUNC(test_syscalls_filehandling) {
    struct mips mips     = {0};
    struct memory memory = {0};

    mips.r[MIPS_R_PC] = PROGRAM_START;

    /* allocate file name */
    const char *filename     = FILENAME;
    const u32   filename_len = strlen(filename)+1;

    /* allocate message */
    const char *message     = MESSAGE;
    const u32   message_len = strlen(message);

    /* load the read and write functions */
    memory.read  = memory_le_read;
    memory.write = memory_le_write;

    /* create program segment */
    struct segment *program =
        create_segment(&memory, PROGRAM_START,
                                PROGRAM_START+PROGRAM_SIZE,
                                PROGRAM_PERM,
                                PROGRAM_SIZE);
    assert(program);
    /* create ram segment */
    struct segment *ram =
        create_segment(&memory, RAM_START,
                                RAM_START+filename_len+message_len,
                                RAM_PERM,
                                filename_len+message_len);
    assert(ram);

    /* copy filename into creeated segment */
    for (u32 b = FILENAME_POINTER; b < FILENAME_POINTER + filename_len; b++)
        ram->segment[b-ram->lower] = filename[b-FILENAME_POINTER];

    /* copy message into creeated segment */
    for (u32 b = MESSAGE_POINTER; b < MESSAGE_POINTER + message_len; b++)
        ram->segment[b-ram->lower] = message[b-MESSAGE_POINTER];

    {
        const u32 sequence[] = {
            /* OPEN "file.txt"
             * addiu %a0, %zr, FILENAME_POINTER
             * addiu %a1, %zr, FILENAME_FLAGS
             * syscall
             * addu  %a0, %zr, %v0
             */
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_V0, MIPS_SYSCALL_OPEN),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A0, FILENAME_POINTER),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A1, FILEFLAGS),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A2, 0755),
            ASSEMBLE_SYSCALL(SYSCALL_FN, 0),
            ASSEMBLE_ALU_REG(ADDU_FN,  MIPS_R_V0, MIPS_R_ZERO, MIPS_R_A0),
            ASSEMBLE_HALT,
        };

        tload(&memory, mips.r[MIPS_R_PC], 
              sizeof(sequence), sequence);

        TEST_EXECUTE;

        TEST_ASSERT(mips.r[MIPS_R_V0] != (u32) -1, "filehandling: open",
                "%%v0>=0", "%%v0=%d when filename=%s, mode=%d",
                mips.r[MIPS_R_V0], filename, FILEFLAGS);
    }
        
    {
        const u32 sequence[] = {
            /* WRITE "Hello World!" to "file.txt"
             * addiu %a1, %zr, MESSAGE_POINTER
             * addiu %a2, %zr, MESSAGE_LENGTH
             * syscall
             */
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_V0, MIPS_SYSCALL_WRITE),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A1, MESSAGE_POINTER),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A2, MESSAGE_LENGTH),
            ASSEMBLE_SYSCALL(SYSCALL_FN, 0),
            ASSEMBLE_HALT,
        };

        tload(&memory, mips.r[MIPS_R_PC], 
              sizeof(sequence), sequence);

        TEST_EXECUTE;

        TEST_ASSERT(mips.r[MIPS_R_V0] == message_len, "filehandling: write",
                "%%v0=%d", "%%v0=%d when len=%d, msg=%s",
                message_len, mips.r[MIPS_R_V0], message_len, message);
    }

    {
        const u32 sequence[] = {
            /* LSEEK to start of "file.txt"
             * addiu %a1, %zr, 0
             * addiu %a2, %zr, SEEK_SET
             * syscall
             */
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_V0, MIPS_SYSCALL_LSEEK),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A1, 0),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A2, SEEK_SET),
            ASSEMBLE_SYSCALL(SYSCALL_FN, 0),
            ASSEMBLE_HALT,
        };

        tload(&memory, mips.r[MIPS_R_PC], 
              sizeof(sequence), sequence);

        TEST_EXECUTE;

        TEST_ASSERT(mips.r[MIPS_R_V0] == 0, "filehandling: lseek",
                "%%v0=0", "%%v0=%d when offset=0, whence=SEEK_SET",
                mips.r[MIPS_R_V0]);
    }

    {
        const u32 sequence[] = {
            /* READ  "Hello World!" from "file.txt"
             * addiu %a1, %zr, MESSAGE_POINTER
             * addiu %a2, %zr, MESSAGE_LENGTH
             * syscall
             */
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_V0, MIPS_SYSCALL_READ),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A1, MESSAGE_POINTER),
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_A2, MESSAGE_LENGTH),
            ASSEMBLE_SYSCALL(SYSCALL_FN, 0),
            ASSEMBLE_HALT,
        };

        tload(&memory, mips.r[MIPS_R_PC], 
              sizeof(sequence), sequence);
        
        TEST_EXECUTE;

        TEST_ASSERT(mips.r[MIPS_R_V0] == message_len, "filehandling: read",
                "%%v0=%d", "%%v0=%d when len=%d, msg=%s",
                message_len, mips.r[MIPS_R_V0], message_len, message);
    }

    {
        const u32 sequence[] = {
            /* CLOSE "file.txt"
             * syscall
             */
            ASSEMBLE_ALU_IMM(ADDIU_OP, MIPS_R_ZERO, MIPS_R_V0, MIPS_SYSCALL_CLOSE),
            ASSEMBLE_SYSCALL(SYSCALL_FN, 0),
            ASSEMBLE_HALT,
        };

        tload(&memory, mips.r[MIPS_R_PC], 
              sizeof(sequence), sequence);
        
        TEST_EXECUTE;

        TEST_ASSERT(mips.r[MIPS_R_V0] == 0, "filehandling: close",
                "%%v0=0", "%%v0=%d when fd=%d",
                mips.r[MIPS_R_V0], mips.r[MIPS_R_A0]);
    }

    delete_memory(&memory);
}

#undef PROGRAM_START
#undef PROGRAM_SIZE 
#undef PROGRAM_PERM 

#undef RAM_START
#undef RAM_PERM 

#undef FILENAME 
#undef FILEFLAGS
#undef FILENAME_LENGTH  
#undef FILENAME_POINTER
#undef  MESSAGE_POINTER 
#undef  MESSAGE_LENGTH 
#undef  MESSAGE 

#endif // __TEST_SYSCALLS__

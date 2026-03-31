#ifndef __MIPS_SYSCALLS__
#define __MIPS_SYSCALLS__

#include <sys/ioctl.h>

/*
 */
static inline void mips_syscall_exit(struct mips *mips, struct memory *memory,
                                     u32 status, u32 _a1, u32 _a2, u32 _a3) {
    mips->status = status;
    mips->halted = 1;
}

/* \brief syscall to read file descriptor
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> fd       - file descriptor
 * \param %a1 -> address  - address of buffer in virtual memory
 * \param %a2 -> size     - size of buffer
 */
static inline void mips_syscall_read(struct mips *mips, struct memory *memory,
                                     u32 fd, u32 address, u32 size, u32 _a3) {
    (void) _a3;

    void *buffer = malloc(size);
    if (!buffer) {
        // handle error
        assert(0);
    }

    mips->r[MIPS_R_V0] = 
        read(fd, buffer, size);

    memory_copy_write(memory, address, 
                      size, buffer);

    free(buffer);
}

/* \brief syscall to write to file descriptor
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> fd       - file descriptor
 * \param %a1 -> address  - address of buffer in virtual memory
 * \param %a2 -> size     - size of buffer
 */
static inline void mips_syscall_write(struct mips *mips, struct memory *memory,
                                      u32 fd, u32 address, u32 size, u32 _a3) {
    (void) _a3;

    void *buffer = malloc(size);
    if (!buffer) {
        // handle error
        assert(0);
    }

    memory_copy_read(memory, address,
                     size, buffer);

    mips->r[MIPS_R_V0] =
        write(fd, buffer, size);

    free(buffer);
}

/* \brief syscall to open a file descriptor
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> name  - address to where the file name is located
 * \param %a1 -> flags - state in which to open the file
 * \param %a1 -> mode  - permissions of the file
 */
static inline void mips_syscall_open(struct mips *mips, struct memory *memory,
                                     u32 name, u32 flags, u32 mode, u32 _a3) {
    (void) _a3;

    // find string length
    u8 c; u32 size = 0;
    do {
        memory_read_u8(memory, 
            name+size++, &c);
    } while(c != '\0');

    // allocate for filename
    const char *buffer = malloc(size);
    if (!buffer) {
        // handle error
        assert(0);
    }
    
    // copy filename
    memory_copy_read(memory, name, 
                     size, (void*) buffer);

    // process open syscall
    mips->r[MIPS_R_V0] = (flags & O_CREAT) ?
        open(buffer, flags, mode & 0777):
        open(buffer, flags);

    // free filename buffer
    free((void*) buffer);
}


/* \brief syscall to close a file descriptor
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> fd - file descriptor
 */
static inline void mips_syscall_close(struct mips *mips, struct memory *memory,
                                      u32 fd, u32 _a1, u32 _a2, u32 _a3) {
    (void) _a1; (void) _a2; (void) _a3;
    /* call host and store results in v0 */
    mips->r[MIPS_R_V0] = close(fd);
}

/* \brief syscall to close a file descriptor
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> fd     - file descriptor
 * \param %a1 -> offset - offset from whence
 * \param %a2 -> whence - directive, SEEK_SET(start), SEEK_CUR(current), SEEK_END(end)
 */
static inline void mips_syscall_lseek(struct mips *mips, struct memory *memory,
                                      u32 fd, u32 offset, u32 whence, u32 _a3) {
    (void) _a3;
    /* call host and store results in v0 */
    mips->r[MIPS_R_V0] = lseek(fd, offset, whence);
}

/* \breif syscall to access ioctl
 *
 * \param fd - file descriptor
 * \param request - request type
 * \param argp -
 */
static inline void mips_syscall_ioctl(struct mips *mips, struct memory *memory, 
                                      u32 fd, u32 request, u32 argp, u32 _a3) {
    (void) _a3;

    switch (request) {
        case 0x40087468: {
            struct winsize ws;

            /* retrieve the host terminal window size */
            assert(ioctl(fd, TIOCGWINSZ, &ws) == 0 &&
                    "failed to get terminal window size");

            /* write the window size to the guest memory */
            memory_write_u16(memory, argp+0, ws.ws_row   );
            memory_write_u16(memory, argp+2, ws.ws_col   );
            memory_write_u16(memory, argp+4, ws.ws_xpixel);
            memory_write_u16(memory, argp+6, ws.ws_ypixel);

            /* success */
            mips->r[MIPS_R_V0] = 0;
            return;
        }
    }

    printf("sys_ioctl: fd=%d, req=0x%08x, argp=0x%08x\n", 
            fd, request, argp);
    assert(0 && 
        "ioctl request no supported");
}

/* \brief syscall to write multiple buffers (vectorized I/O)
 *
 * \param %a0 -> fd      - file descriptor
 * \param %a1 -> iov_ptr - address of the iovec array
 * \param %a2 -> iovcnt  - number of iovec structures
 */
static inline void mips_syscall_writev(struct mips *mips, struct memory *memory, 
                                       u32 fd, u32 ptr, u32 cnt, u32 _a3) {
    (void) _a3;

    u32 total = 0;
    for (u32 c = 0; c < cnt; c++) {
        /* define the iovec */
        struct { 
            u32 address; 
            u32 length; 
        } iovec = {0};

        /* read in iovec */
        memory_read_u32(memory, ptr + c*8 + 0, &iovec.address);
        memory_read_u32(memory, ptr + c*8 + 4, &iovec.length);

        if (iovec.length) {
            // allocate memort for string
            void *buffer = malloc(iovec.length);
            if (!buffer) {
                // handle error
                assert(0);
            }

            // copy string from memory
            memory_copy_read(memory, iovec.address, 
                             iovec.length, buffer);

            // write to file descriptor
            ssize_t result = 
                write(fd, buffer, iovec.length);

            // add sum to total
            total += result;

            // free buffer
            free(buffer);
        }
    }

    mips->r[MIPS_R_V0] = total;
}

static inline void mips_syscall_brk(struct mips *mips, struct memory *memory,
                                    u32 brk, u32 _a1, u32 _a2, u32 _a3) {
    (void)_a1; (void)_a2; (void)_a3;

    if (brk > memory->brk_heap_end) {
        // allocate requested space on brk heap
        memory_allocate(memory, memory->brk_heap_end, 
                            brk-memory->brk_heap_end);
        // move brk heap end pointer
        memory->brk_heap_end = brk;
    }

    mips->r[MIPS_R_V0] = memory->brk_heap_end;
}

static inline void mips_syscall_mmap2(struct mips *mips, struct memory *memory,
                                      u32 addr, u32 len, u32 prot, u32 flags) {
    // retrieve all stack arguments
    u32 fd, pgoff;
    memory_read_u32(memory, mips->r[MIPS_R_SP]+16, &fd   );
    memory_read_u32(memory, mips->r[MIPS_R_SP]+20, &pgoff);

    // align len for mmap segment
    len = (len + 0xfff) & ~0xfff;
    
    // get end of current mmap region
    u32 target_address = 
        memory->mmap_heap_end;

    // check for overflow
    if (target_address + len > USER_TOP) {
        // handle error
        assert(0);
    }

    // allocate memory
    memory_allocate(memory, 
        memory->mmap_heap_end, len);

    // clear memory
    memory_set(memory,
        memory->mmap_heap_end, len, 0);
    
    // increment mmap heap end
    memory->mmap_heap_end += len;
    
    // return start and set error to 0
    mips->r[MIPS_R_V0] = target_address;
    mips->r[MIPS_R_A3] = 0;
}

static inline void mips_syscall_munmap(struct mips *mips, struct memory *memory,
                                       u32 addr, u32 len, u32 _a2, u32 _a3) {
    (void) _a2; (void) _a3;
    
    // memory_deallocate(memory, addr, len);

    mips->r[MIPS_R_V0] = 0;
    mips->r[MIPS_R_A0] = 0;
}

/* \brief syscall to set protection on a region of memory
 *
 * %a0 -> addr - starting address (must be page-aligned)
 * %a1 -> len  - size of the region
 * %a2 -> prot - PROT_READ(1), PROT_WRITE(2), PROT_EXEC(4)
 */
static inline void mips_syscall_mprotect(struct mips *mips, struct memory *memory,
                                         u32 addr, u32 len, u32 prot, u32 _a3) {
    (void)memory; (void)addr; (void)len; (void)prot; (void)_a3;

#ifdef __DISASSEMBLE__
    printf("sys_mprotect: addr=0x%08x, len=%u, prot=%d (Faking Success)\n",
            addr, len, prot);
#endif

    /* Most benchmarks don't actually need strict hardware page protection.
     * Returning 0 tells the guest 'Mission Accomplished'. */
    mips->r[MIPS_R_V0] = 0;  // Success
    mips->r[MIPS_R_A3] = 0;  // No error
}

static inline void mips_syscall_clock_gettime64(struct mips *mips, struct memory *memory,
                                                u32 clock_id, u32 timespec_ptr, u32 _a2, u32 _a3) {
    (void) _a2; (void) _a3;

    mips->r[MIPS_R_V0] = 22;
    mips->r[MIPS_R_A3] =  1;

    struct timespec host_ts;
    
    // get host time
    if (clock_gettime(clock_id, &host_ts) == 0) {
        // write seconds to guest structure
        memory_write_u32(memory, timespec_ptr +  0, (u32)(host_ts.tv_sec >>  0)); // tv_sec
        memory_write_u32(memory, timespec_ptr +  4, (u32)(host_ts.tv_sec >> 32)); // tv_sec

        // write nano seconds to guest structure
        memory_write_u32(memory, timespec_ptr +  8, (u32)(host_ts.tv_nsec >>  0)); // tv_sec
        memory_write_u32(memory, timespec_ptr + 12, (u32)(host_ts.tv_nsec >> 32)); // tv_sec

        // set success
        mips->r[MIPS_R_V0] = 0;
        mips->r[MIPS_R_A3] = 0;
    }
}

/* \brief syscall to terminate all threads in a process
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> status - the exit status for the entire process
 */
static inline void mips_syscall_exit_group(struct mips *mips, struct memory *memory,
                                           u32 status, u32 _a1, u32 _a2, u32 _a3) {
    mips->status = status;
    mips->halted = 1;
}

/* \brief syscall to terminate all threads in a process
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> status - the exit status for the entire process
 */
static inline void mips_syscall_set_thread_area(struct mips *mips, struct memory *memory,
                                                u32 addr, u32 _a1, u32 _a2, u32 _a3) {
    // Custom field in your struct
    mips->user_local_ptr = addr;
    mips->r[MIPS_R_V0] = 0;      // Success
}

/* \brief syscall to terminate all threads in a process
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> status - the exit status for the entire process
 */
static inline void mips_syscall_tid_addr(struct mips *mips, struct memory *memory,
                                         u32 tidptr, u32 _a1, u32 _a2, u32 _a3) {
    // store the tidptr
    mips->tidptr = tidptr;
    // Since running a single process, returning '1' is standard.
    mips->r[MIPS_R_V0] = 1;
}

#endif // __MIPS_SYSCALLS__


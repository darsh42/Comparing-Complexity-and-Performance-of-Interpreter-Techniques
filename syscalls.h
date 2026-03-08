#ifndef __MIPS_SYSCALLS__
#define __MIPS_SYSCALLS__

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
    /* get segment where buffer is located */
    struct segment *segment = 
        memory_map_address(memory, address);
    /* compute offset into segment */
    address -= segment->lower;
    /* call host and store results in %v0 */
    mips->r[MIPS_R_V0] = 
        read(fd, (void *) (segment->segment+address), size);
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
    /* get segment where buffer is located */
    struct segment *segment = 
        memory_map_address(memory, address);
    /* compute offset into segment */
    address -= segment->lower;
    /* call host and store results in %v0 */
    mips->r[MIPS_R_V0] = 
        write(fd, (void *) (segment->segment+address), size);
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
    /* get segment where the filename is stored */
    struct segment *segment = 
        memory_map_address(memory, name);
    /* compute offset into segment */
    name -= segment->lower;
    /* call host and store results in v0 */
    mips->r[MIPS_R_V0] = (flags & O_CREAT) ?
        open((char *) segment->segment+name, flags, mode & 0777):
        open((char *) segment->segment+name, flags);
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

/* \brief syscall to terminate all threads in a process
 *
 * \param mips   pointer to instance mips
 * \param memory pointer to instance memory
 *
 * \param %a0 -> status - the exit status for the entire process
 */
static inline void mips_syscall_exit_group(struct mips *mips, struct memory *memory,
                                           u32 _a0, u32 _a1, u32 _a2, u32 _a3) {
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


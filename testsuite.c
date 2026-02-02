#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>

#include "log.h"
#include "test.h"
#include "mips.h"
#include "memory.h"

/*
 * basic loader for assembled mneumonics
 */
void tload(struct memory *memory, u32 address, size_t count, const u32 *instructions) {
    struct segment *segment = 
        memory_map_address(memory, address);

    if (segment == NULL) {
        /* load the read and write functions */
        memory->read  = memory_le_read;
        memory->write = memory_le_write;

        /* create a new segment */
        segment = create_segment(memory,
                                 address, 
                                 address + count,
                                 256, count);

    }

    /* ensure there is a segment */
    assert(segment);

    /* translate address to segment base */
    address -= segment->lower;

    /* load instructions to segment */
    for (size_t b = 0; b < count; b++) {
        *(segment->segment + address + b) = 
            ((u8 *) instructions)[b];
    }
}

/*
 * basic test assert
 */
void tassert(bool condition, const char *name, const char *diag, ...) {

#define T_NORM  "\033[0m"
#define T_RED   "\033[91m"
#define T_GREEN "\033[32m"

    va_list args;

    va_start(args, diag);
    fprintf(stderr, "%s: %s\n", 
        (condition) ? T_GREEN "[PASSED]" T_NORM:
                      T_RED   "[FAILED]" T_NORM,
        name);
    if (!condition) {
        vfprintf(stderr, diag, args);
    }

    va_end(args);
}

/* interpreter routines */
extern void interpreter_switch(
        struct mips *mips, struct memory *memory);
extern void interpreter_computed_goto(
        struct mips *mips, struct memory *memory);
extern void interpreter_decoupled(
        struct mips *mips, struct memory *memory);

int main(int argc, char **argv) {
    /* maybe put test selection */
    (void) argc;
    (void) argv;

    fprintf(stderr, "SWITCH MODEL TEST SUITE\n");
    for (auto c : tests)
        c(&interpreter_switch);

    fprintf(stderr, "COMPUTED MODEL TEST SUITE\n");
    for (auto c : tests)
        c(&interpreter_computed_goto);

    fprintf(stderr, "DECOUPLED MODEL TEST SUITE\n");
    for (auto c : tests)
        c(&interpreter_decoupled);

    return 0;
}

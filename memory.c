#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <elf.h>
#include <assert.h>

#include "log.h"
#include "memory.h"

#define MEMORY_RAM_FLAGS PF_X | PF_W | PF_R
#define MEMORY_RAM_SIZE  8192
#define MEMORY_RAM_BOT   0
#define MEMORY_RAM_TOP   MEMORY_RAM_BOT + MEMORY_RAM_SIZE - 1

struct segment *create_segment(struct memory *memory, u32 lower, 
                               u32 upper, u32 permissions, size_t size) {
    /* allocate the segment */
    memory->segments_count++;
    memory->segments = realloc(memory->segments, 
            sizeof(struct segment) * memory->segments_count);
    
    assert(memory->segments);

    /* retrive the segment pointer so it can be populated */
    struct segment *segment = 
        &memory->segments[memory->segments_count - 1];
    
    /* set segment upper and lower bounds */
    segment->lower = lower;
    segment->upper = upper;

    /* set the segment permissions */
    segment->permissions = permissions;

    /* allocate space for data */
    segment->segment = calloc(1, size);

    /* return pointer to segment */
    return segment;
}

void delete_segment(struct memory *memory, size_t index) {
    assert(index < memory->segments_count);
    free(memory->segments[index].segment);
}

void print_segment(struct memory *memory, size_t index) {
    assert(index < memory->segments_count);
    printf("[%2zu] lower: %08x, upper: %08x, perm: %d\n", index, 
            memory->segments[index].lower, memory->segments[index].upper, 
            memory->segments[index].permissions);
}

/*******************************************
 * ENDIANNESS - MIPS architecture supports *
 *              big and little endian add- *
 *              ressing schemes.           *
 *                                         *
 *              Therefor reading and writ- *
 *              ing is changed based on    *
 *              the elf that is passed.    *
 *******************************************/
/* read big endian */
void memory_be_read(struct segment *segment, u32 address, u32 *data, u32 size) {
    u32 _data = 0;
    switch (size) {
    case 4: _data |= *(segment->segment + address + 0) << 24;
            _data |= *(segment->segment + address + 1) << 16;
    case 2: _data |= *(segment->segment + address + 2) <<  8;
    case 1: _data |= *(segment->segment + address + 3) <<  0;
    } *data = _data;
}

/* read little endian */
void memory_le_read(struct segment *segment, u32 address, u32 *data, u32 size) {
    u32 _data = 0;
    switch (size) {
    case 4: _data |= *(segment->segment + address + 3) << 24;
            _data |= *(segment->segment + address + 2) << 16;
    case 2: _data |= *(segment->segment + address + 1) <<  8;
    case 1: _data |= *(segment->segment + address + 0) <<  0;
    } *data = _data;
}

/* write big endian */
void memory_be_write(struct segment *segment, u32 address, u32 data, u32 size) {
    switch (size) {
    case 4: *(segment->segment + address + 0) = (uint8_t) (data >> 24);
            *(segment->segment + address + 1) = (uint8_t) (data >> 16); 
    case 2: *(segment->segment + address + 2) = (uint8_t) (data >>  8);
    case 1: *(segment->segment + address + 3) = (uint8_t) (data >>  0);
    }
}

/* write little endian */
void memory_le_write(struct segment *segment, u32 address, u32 data, u32 size) {
    switch (size) {
    case 4: *(segment->segment + address + 3) = (uint8_t) (data >> 24);
            *(segment->segment + address + 2) = (uint8_t) (data >> 16); 
    case 2: *(segment->segment + address + 1) = (uint8_t) (data >>  8);
    case 1: *(segment->segment + address + 0) = (uint8_t) (data >>  0);
    }
}

/*******************************************
 * READ/WRITE - Basic methods for reading  *
 *              and writing to the memory  *
 *              of the emulated processor. *
 *                                         *
 *              Each operation checks for  *
 *              memory bounds and maps ad- *
 *              resses based on the virtu- *
 *              al addressing scheme.      *
 *******************************************/
struct segment *memory_map_address(struct memory *memory, u32 address) {
    for (size_t s = 0; s < memory->segments_count; s++) {
        if (address >= memory->segments[s].lower &&
            address <= memory->segments[s].upper) {
            return &memory->segments[s];
        }
    }
    return NULL;
}
void memory_read(struct memory *memory, u32 address, u32 *data, u32 size) {
    /* define segment holder */
    struct segment *segment = 
        memory_map_address(memory, address);
    assert(segment != NULL);

    /* translate address to segment base */
    address -= segment->lower;

    /* check memory index validity */
    assert(address <= segment->upper);

    /* depending on endianness store value */
    memory->read(segment, address, data, size); 
}

void memory_write(struct memory *memory, u32 address, u32 data, u32 size) {
    /* define segment holder */
    struct segment *segment = 
        memory_map_address(memory, address);
    assert(segment != NULL);

    /* translate address to segment base */
    address -= segment->lower;

    /* check memory index validity */
    assert(address <= segment->upper);

    /* depending on endianness store value */
    memory->write(segment, address, data, size);
}

void create_memory(struct memory *memory) {
    /* ensure memory is empty */
    assert(memory->read == NULL);
    assert(memory->write == NULL);
    assert(memory->segments == NULL);
    assert(memory->segments_count == 0);

    /* create RAM */
    create_segment(memory, MEMORY_RAM_BOT,
                           MEMORY_RAM_TOP,
                           MEMORY_RAM_FLAGS,
                           MEMORY_RAM_SIZE);
}

void delete_memory(struct memory *memory) {
    /* delete individual segment memories */
    for (size_t s = 0; s < memory->segments_count; s++) {
        delete_segment(memory, s);
    }

    /* delete segments arrays */
    free(memory->segments);
}


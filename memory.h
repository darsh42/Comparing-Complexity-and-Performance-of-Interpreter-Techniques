#ifndef __MEMORY_H__
#define __MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef  int64_t s64;
typedef  int32_t s32;
typedef  int16_t s16;
typedef   int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef  uint8_t  u8;


struct segment {
    /* lowest and highest address */
    u32 lower, upper;
    /* read/write/execute permissions */
    u32 permissions;
    /* segment address */
    u8 *segment;
};

typedef void (*fn_read) (struct segment *, u32, u32*, u32);
typedef void (*fn_write)(struct segment *, u32, u32,  u32);

struct memory {
    /* read and writers based on endianess */
    fn_read  read;
    fn_write write;

    /* memory sections and the address ranges */
    struct segment *segments;
    size_t          segments_count;
};

struct segment *create_segment(struct memory *memory, u32 lower, 
                               u32 upper, u32 permissions, size_t size);
void delete_segment(struct memory *memory, size_t index);
void  print_segment(struct memory *memory, size_t index);

void memory_be_read(struct segment *segment, u32 address, u32 *data, u32 size);
void memory_be_write(struct segment *segment, u32 address, u32 data, u32 size);
void memory_le_read(struct segment *segment, u32 address, u32 *data, u32 size);
void memory_le_write(struct segment *segment, u32 address, u32 data, u32 size);

struct segment *memory_map_address(struct memory *memory, u32 address);
void memory_read(struct memory *memory, u32 address, u32 *data, u32 size);
void memory_write(struct memory *memory, u32 address, u32 data, u32 size);

void create_memory(struct memory *memory);
void delete_memory(struct memory *memory);

#ifdef __cplusplus
}
#endif

#endif // __MEMORY_H__

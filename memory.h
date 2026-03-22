#ifndef __MEMORY_H__
#define __MEMORY_H__

typedef  int64_t s64;
typedef  int32_t s32;
typedef  int16_t s16;
typedef   int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef  uint8_t  u8;

#define USER_TOP  0x80000000

#define STACK_PERM 255
#define STACK_SIZE 0x10000
#define STACK_TOP USER_TOP
#define STACK_BOT STACK_TOP - STACK_SIZE

struct segment {
    /* lowest and highest address */
    u32 lower, upper;
    /* read/write/execute permissions */
    u32 permissions;
    /* segment address */
    u8 *segment;
};

struct memory {
    /* heap addresses */
    u32 heap_start;
    u32 heap_end;

    /* memory sections and the address ranges */
    struct segment *segments;
    size_t          segments_count;
};

struct segment *create_segment(struct memory *memory, u32 lower, 
                               u32 upper, u32 permissions, size_t size);
void delete_segment(struct memory *memory, size_t index);
void  print_segment(struct memory *memory, size_t index);
struct segment *memory_map_address(struct memory *memory, u32 address);
void memory_read(struct memory *memory, u32 address, u32 *data, u32 size);
void memory_write(struct memory *memory, u32 address, u32 data, u32 size);
u32 memory_read_chunk(struct memory *memory, u32 address, u32 *data, u32 size);
void create_memory(struct memory *memory);
void delete_memory(struct memory *memory);

#endif // __MEMORY_H__

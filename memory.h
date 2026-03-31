#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#define USER_TOP  0x80000000
#define STACK_SIZE 0x10000
#define STACK_TOP USER_TOP
#define STACK_BOT STACK_TOP - STACK_SIZE

#define ADDRESS_WIDTH 32
#define PT_SHIFT 22
#define PE_SHIFT 12

#define PT_MASK 0b1111111111
#define PE_MASK 0b1111111111

#define PT_COUNT 1 << (ADDRESS_WIDTH - PT_SHIFT)
#define PE_COUNT 1 << (     PT_SHIFT - PE_SHIFT)

#define OFFSET_MASK 0b111111111111

typedef void * page_entry_t;

typedef struct page_table {
    page_entry_t page_entries[PE_COUNT];
} __attribute__((alignas(64))) page_table_t;

typedef struct memory {
    page_table_t *page_tables[PT_COUNT];

    u32  brk_heap_start,  brk_heap_end;
    u32 mmap_heap_start, mmap_heap_end;
}memory_t;

void memory_create(memory_t *memory);
void memory_delete(memory_t *memory);

void memory_allocate(memory_t *memory, u32 address, u32 size);
void memory_set(memory_t *memory, u32 address, u32 size, u32 value);

void memory_copy_read (memory_t *memory, u32 address, u32 size, void *buffer);
void memory_copy_write(memory_t *memory, u32 address, u32 size, void *buffer);

void memory_read(memory_t *memory, u32 address, void *data, u32 size);
void memory_read_u8 (memory_t * restrict memory, u32 address, u8  * restrict destination);
void memory_read_u16(memory_t * restrict memory, u32 address, u16 * restrict destination);
void memory_read_u32(memory_t * restrict memory, u32 address, u32 * restrict destination);

void memory_write(memory_t *memory, u32 address, u32   data, u32 size);
void memory_write_u8 (memory_t * restrict memory, u32 address, const u8  data);
void memory_write_u16(memory_t * restrict memory, u32 address, const u16 data);
void memory_write_u32(memory_t * restrict memory, u32 address, const u32 data);


#endif // __MEMORY_H__


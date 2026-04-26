#include "memory.h"

void memory_allocate(memory_t *memory, u32 address, u32 size) {
    /*
     * page boundaries:
     * |____|____|____|____|____|____|____|____|____|____|
     *
     * allocation: H - head, A - aligned, T- tail
     * |______|HHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT|_______|
     *
     * align head and tail
     * aligned_head = beg_addr >> PAGE_SHIFT;
     * aligned_tail = end_addr >> PAGE_SHIFT;
     *
     * page_count = (aligned_tail - aligned_head) / page_size;
     */
    
    u32 aligned_table_head = ((address + 0)        >> PT_SHIFT) & PT_MASK;
    u32 aligned_table_tail = ((address + size - 1) >> PT_SHIFT) & PT_MASK;

    for (u32 t = aligned_table_head; t <= aligned_table_tail; t++) {
        // get table index
        u32 pt_idx = t & PT_MASK;

        // find table
        page_table_t *table = memory->page_tables[pt_idx];
        if (!table) {
            /* allocate a new table entry */
            table = calloc(1, sizeof(page_table_t));
            if (!table) {
                fprintf(stderr, "failed to allocate table");
                goto err;
            }
            memory->page_tables[pt_idx] = table;
        }

        u32 entry_start = 0;
        u32 entry_end   = PE_MASK;

        // if the entry is in head table 
        if (t == aligned_table_head) {
            entry_start = 
                ((address + 0) >> PE_SHIFT) & PE_MASK;
        }

        // if the entry is in tail table
        if (t == aligned_table_tail) {
            entry_end = 
                ((address + size - 1) >> PE_SHIFT) & PE_MASK;
        }

        for (u32 e = entry_start; e <= entry_end; e++) {
            u32 pe_idx = e & PE_MASK;

            page_entry_t  entry = table->page_entries[pe_idx];
            if (!entry) {
                /* allocate a new page entry */
                entry = calloc(1, 1 << PE_SHIFT);
                if (!entry) {
                    fprintf(stderr, "failed to allocate page\n");
                    goto err;
                }
                table->page_entries[pe_idx] = entry;
            }
        }
    }

    return;

err:
    abort();
}

void memory_copy_read(memory_t *memory, u32 address, u32 size, void *buffer) {
    u8* _buffer = buffer;
    while (size) {
        /* pointer to memory location */
        u8* location = memory_find_page(memory, address);

        /* calculate amount of bytes to copy in this page */
        u32 offset = address & OFFSET_MASK;
        u32 amount = (size < OFFSET_MASK+1 - offset) ?
            size: OFFSET_MASK+1 - offset;

        memcpy(_buffer, location, amount);

        /* increment to next page */
        address += amount;
        _buffer += amount;
        size    -= amount;
    }

    return;

err:
    abort();
}

void memory_copy_write(memory_t *memory, u32 address, u32 size, void *buffer) {
    u8* _buffer = buffer;
    while (size) {
        /* pointer to memory location */
        u8* location = memory_find_page(memory, address);

        /* calculate amount of bytes to copy in this page */
        u32 offset = address & OFFSET_MASK;
        u32 amount = (size < OFFSET_MASK+1 - offset) ?
            size: OFFSET_MASK+1 - offset;

        memcpy(location, _buffer, amount);

        /* increment to next page */
        address += amount;
        _buffer += amount;
        size    -= amount;
    }
}

void memory_set(memory_t *memory, u32 address, u32 size, u32 value) {
    while (size) {
        /* pointer to memory location */
        u8* location = memory_find_page(memory, address);

        /* calculate amount of bytes to copy in this page */
        u32 offset = address & OFFSET_MASK;
        u32 amount = (size < OFFSET_MASK+1 - offset) ?
            size: OFFSET_MASK+1 - offset;

        memset(location, value, amount);

        /* increment to next page */
        address += amount;
        size    -= amount;
    }
}

void memory_create(memory_t *memory) {
    assert(memory);

    /* set mmap heap */
    memory->mmap_heap_start = 0x40000000;
    memory->mmap_heap_end   = 0x40000000;
}

void memory_delete(memory_t *memory) {
    for (u32 t = 0; t < PT_COUNT; t++) {
        // get page table
        page_table_t *table = 
            memory->page_tables[t];

        // skip if un-alloced
        if (table == NULL) {
            continue;
        }

        // free page entries
        for (u32 e = 0; e < PE_COUNT; e++) {
            // get page entry
            page_entry_t entry = 
                table->page_entries[e];

            // skip if un-alloced
            if (entry == NULL) {
                continue;
            }

            // free entry
            free(entry);
        }

        // free table
        free(table);
    }
}

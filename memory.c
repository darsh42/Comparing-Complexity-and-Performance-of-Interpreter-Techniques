#include "memory.h"

u8 *memory_find_page(memory_t *memory, u32 address) {
    // get table index, entry index and offset
    u32 pt_idx = (address >> PT_SHIFT) & PT_MASK;
    u32 pe_idx = (address >> PE_SHIFT) & PE_MASK;
    u32 offset = address & OFFSET_MASK;

    // find table
    page_table_t *table = memory->page_tables[pt_idx];
    // assert(table && "[memory_find_page]: invalid table");

    // find entry
    page_entry_t  entry = table->page_entries[pe_idx];
    // assert(entry && "[memory_find_page]: invalid entry");

    // compute source location
    return (u8 *)entry + offset;
}

void memory_read_u8 (memory_t * restrict memory, u32 address, u8  * restrict destination) {
    *destination = *memory_find_page(memory, address);
}
void memory_read_u16(memory_t * restrict memory, u32 address, u16 * restrict destination) {
    *destination = *(u16 *) memory_find_page(memory, address);
}
void memory_read_u32(memory_t * restrict memory, u32 address, u32 * restrict destination) {
    *destination = *(u32 *) memory_find_page(memory, address);
}
void memory_write_u8 (memory_t * restrict memory, u32 address, const u8  data) {
    *memory_find_page(memory, address) = data;
}
void memory_write_u16(memory_t * restrict memory, u32 address, const u16 data) {
    *(u16 *)memory_find_page(memory, address) = data;
}
void memory_write_u32(memory_t * restrict memory, u32 address, const u32 data) {
    *(u32 *)memory_find_page(memory, address) = data;
}

void memory_read(memory_t *memory, u32 address, void *data, u32 size) {
    assert(memory && data);

    // get table index, entry index and offset
    u32 pt_idx = (address >> PT_SHIFT) & PT_MASK;
    u32 pe_idx = (address >> PE_SHIFT) & PE_MASK;
    u32 offset = address & OFFSET_MASK;

    // find table
    page_table_t *table = memory->page_tables[pt_idx];
    assert(table && "[memory_read]: invalid read\n");

    // find entry
    page_entry_t  entry = table->page_entries[pe_idx];
    assert(entry && "[memory_read]: invalid read\n");

    // compute source location
    u8 *source = (u8 *)entry + offset;

    // perform read
    switch (size) {
    case 4: *(u32 *)data = *(u32 *)source; return;
    case 2: *(u16 *)data = *(u16 *)source; return;
    case 1: *(u8  *)data = *(u8  *)source; return;
    }
}

void memory_write(struct memory *memory, u32 address, u32 data, u32 size) {
    assert(memory);

    // get table index, entry index and offset
    u32 pt_idx = (address >> PT_SHIFT) & PT_MASK;
    u32 pe_idx = (address >> PE_SHIFT) & PE_MASK;
    u32 offset = address & OFFSET_MASK;

    // find table
    page_table_t *table = memory->page_tables[pt_idx];
    assert(table && "[memory_read]: invalid write\n");

    // find entry
    page_entry_t  entry = table->page_entries[pe_idx];
    assert(entry && "[memory_read]: invalid write\n");

    // compute destination location
    u8 *destination = (u8 *)entry + offset;

    // perform read
    switch (size) {
    case 4: *(u32 *) destination = data; return;
    case 2: *(u16 *) destination = data; return;
    case 1: *(u8  *) destination = data; return;
    }
}

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
            assert(table && 
                "failed to allocate table");
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
                assert(entry && 
                    "failed to allocate page");
                table->page_entries[pe_idx] = entry;
            }
        }
    }
}

void memory_copy_read(memory_t *memory, u32 address, u32 size, void *buffer) {
    u8* _buffer = buffer;
    while (size) {
        // get table index, entry index and offset
        u32 pt_idx = (address >> PT_SHIFT) & PT_MASK;
        u32 pe_idx = (address >> PE_SHIFT) & PE_MASK;
        u32 offset =  address & OFFSET_MASK;

        // find table
        page_table_t *table = memory->page_tables[pt_idx];
        if (!table) {
            // handle error
            assert(0);
        }

        // find entry
        page_entry_t  entry = table->page_entries[pe_idx];
        if (!entry) {
            // handle error
            assert(0);
        }

        u32 amount = (size < OFFSET_MASK+1 - offset) ?
            size: OFFSET_MASK+1 - offset;

        memcpy(_buffer, (u8 *)entry+offset, amount);

        address += amount;
        _buffer += amount;
        size    -= amount;
    }
}

void memory_copy_write(memory_t *memory, u32 address, u32 size, void *buffer) {
    u8* _buffer = buffer;
    while (size) {
        // get table index, entry index and offset
        u32 pt_idx = (address >> PT_SHIFT) & PT_MASK;
        u32 pe_idx = (address >> PE_SHIFT) & PE_MASK;
        u32 offset =  address & OFFSET_MASK;

        // find table
        page_table_t *table = memory->page_tables[pt_idx];
        if (!table) {
            // handle error
            assert(0);
        }

        // find entry
        page_entry_t  entry = table->page_entries[pe_idx];
        if (!entry) {
            // handle error
            assert(0);
        }

        u32 amount = (size < OFFSET_MASK+1 - offset) ?
            size: OFFSET_MASK+1 - offset;

        memcpy((u8 *)entry+offset, _buffer, amount);

        address += amount;
        _buffer += amount;
        size    -= amount;
    }
}

void memory_set(memory_t *memory, u32 address, u32 size, u32 value) {
    while (size) {
        // get table index, entry index and offset
        u32 pt_idx = (address >> PT_SHIFT) & PT_MASK;
        u32 pe_idx = (address >> PE_SHIFT) & PE_MASK;
        u32 offset =  address & OFFSET_MASK;

        // find table
        page_table_t *table = memory->page_tables[pt_idx];
        if (!table) {
            // handle error
            assert(0);
        }

        // find entry
        page_entry_t  entry = table->page_entries[pe_idx];
        if (!entry) {
            // handle error
            assert(0);
        }

        u32 amount = (size < OFFSET_MASK+1 - offset) ?
            size: OFFSET_MASK+1 - offset;

        memset((u8 *)entry+offset, value, amount);

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

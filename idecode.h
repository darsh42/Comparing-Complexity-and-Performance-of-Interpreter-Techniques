#ifndef __IDECODE_H__
#define __IDECODE_H__
/* idecode - Instruction Decode Queue
 *
 * DATA STRUCTURE:
 *  Idecode_item {
 *      u32   raw;
 *      void *function
 *  };
 *
 *  Idecode_queue {
 *      // shared 
 *      Idecode_item *data;
 *      u32 capacity;
 *      *pad cache line*
 *      
 *      // producer
 *      u32 head;
 *      *pad cache line*
 *      
 *      // consumer
 *      u32 tail;
 *      *pad cache line*
 *  }
 *
 * METHODS:
 *  enqueue           -> u32 (Q, item )
 *  dequeue           -> u32 (Q, item*)
 *  batch_enqueue     -> u32 (Q, item*)
 *  batch_dequeue     -> u32 (Q, item*)
 *  batch_enqueue_soa -> u32 (Q, u32 *, void **)
 *  batch_dequeue_soa -> u32 (Q, u32 *, void **)
 *  get_length        -> u32 (Q)
 *  wait_space        -> u32 (Q, u32)
 *  wait_taken        -> u32 (Q, u32)
 * */

#include "common.h"
#include "mips.h"

#ifndef likely
# define likely(x)   __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#if defined(__x86_64__) || defined(__i386__)
# define cpu_relax() __asm__ __volatile__("pause")
#else
# define cpu_relax() __asm__ __volatile__("")
#endif

typedef struct {
    u32   opcode;
    void *handle;
} idecode_item_t;

typedef struct {
    idecode_item_t *decoded;
    u32             capacity;
    u32             mask;

    // PAD_TO_CACHELINE;
    u8 _pad1[CACHELINE_SIZE - sizeof(idecode_item_t *) - sizeof(u32) - sizeof(u32)];

    atomic_int head;   // producer only

    // PAD_TO_CACHELINE;
    u8 _pad2[CACHELINE_SIZE - sizeof(atomic_int)];

    atomic_int tail;   // consumer only

    // PAD_TO_CACHELINE;
    u8 _pad3[CACHELINE_SIZE - sizeof(atomic_int)];
} idecode_queue_t;

static inline void
idecode_queue_init(idecode_queue_t *q,
                   idecode_item_t *buffer,
                   u32 capacity)
{
    q->decoded  = buffer;
    q->capacity = capacity;
    q->mask     = capacity - 1;

    atomic_store_explicit(&q->head, 0, memory_order_relaxed);
    atomic_store_explicit(&q->tail, 0, memory_order_relaxed);
}

static inline u32
idecode_get_length(const idecode_queue_t *q)
{
    u32 head = atomic_load_explicit(&q->head, memory_order_acquire);
    u32 tail = atomic_load_explicit(&q->tail, memory_order_acquire);
    return head - tail;
}

static inline u32
idecode_enqueue_soa(idecode_queue_t *q,
                    const u32 opcode,
                    void *handle)
{
    u32 head = atomic_load_explicit(&q->head, memory_order_relaxed);
    u32 tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    if (unlikely(head - tail == q->capacity))
        return 0; // full

    q->decoded[head & q->mask] = 
        (idecode_item_t) { .opcode = opcode, .handle = handle };

    atomic_store_explicit(&q->head, head + 1, memory_order_release);
    return 1;
}
static inline u32
idecode_enqueue(idecode_queue_t *q, const idecode_item_t *item)
{
    u32 head = atomic_load_explicit(&q->head, memory_order_relaxed);
    u32 tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    if (unlikely(head - tail == q->capacity))
        return 0; // full

    q->decoded[head & q->mask] = *item;

    atomic_store_explicit(&q->head, head + 1, memory_order_release);
    return 1;
}

static inline u32
idecode_dequeue(idecode_queue_t *q, idecode_item_t *out)
{
    u32 tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    u32 head = atomic_load_explicit(&q->head, memory_order_acquire);

    if (unlikely(head == tail))
        return 0; // empty

    *out = q->decoded[tail & q->mask];

    atomic_store_explicit(&q->tail, tail + 1, memory_order_release);
    return 1;
}

static inline u32
idecode_batch_enqueue(idecode_queue_t *q,
                      const idecode_item_t *items,
                      u32 count)
{
    u32 head = atomic_load_explicit(&q->head, memory_order_relaxed);
    u32 tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    u32 space = q->capacity - (head - tail);
    if (space == 0)
        return 0;

    if (count > space)
        count = space;

    for (u32 i = 0; i < count; i++)
        q->decoded[(head + i) & q->mask] = items[i];

    atomic_store_explicit(&q->head, head + count, memory_order_release);
    return count;
}

static inline u32
idecode_batch_dequeue(idecode_queue_t *q,
                      idecode_item_t *out,
                      u32 count)
{
    u32 tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    u32 head = atomic_load_explicit(&q->head, memory_order_acquire);

    u32 avail = head - tail;
    if (avail == 0)
        return 0;

    if (count > avail)
        count = avail;

    for (u32 i = 0; i < count; i++)
        out[i] = q->decoded[(tail + i) & q->mask];

    atomic_store_explicit(&q->tail, tail + count, memory_order_release);
    return count;
}

static inline void
idecode_wait_space(const idecode_queue_t *q, u32 n)
{
    for (;;) {
        u32 head = atomic_load_explicit(&q->head, memory_order_relaxed);
        u32 tail = atomic_load_explicit(&q->tail, memory_order_acquire);

        if (q->capacity - (head - tail) >= n)
            return;

        cpu_relax();
    }
}

static inline void
idecode_wait_taken(const idecode_queue_t *q, u32 n)
{
    for (;;) {
        u32 head = atomic_load_explicit(&q->head, memory_order_acquire);
        u32 tail = atomic_load_explicit(&q->tail, memory_order_relaxed);

        if (head - tail >= n)
            return;

        cpu_relax();
    }
}

static inline u32
idecode_batch_enqueue_soa(idecode_queue_t *q,
                          const u32 *opcodes,
                          void *const *handles,
                          u32 count)
{
    u32 head = atomic_load_explicit(&q->head, memory_order_relaxed);
    u32 tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    u32 space = q->capacity - (head - tail);
    if (count > space)
        count = space;
    if (count == 0)
        return 0;

    u32 idx   = head & q->mask;
    u32 first = q->capacity - idx;
    if (first > count)
        first = count;

    for (u32 i = 0; i < first; i++) {
        q->decoded[idx + i].opcode = opcodes[i];
        q->decoded[idx + i].handle = handles[i];
    }

    for (u32 i = first; i < count; i++) {
        q->decoded[i - first].opcode = opcodes[i];
        q->decoded[i - first].handle = handles[i];
    }

    atomic_store_explicit(&q->head, head + count, memory_order_release);
    return count;
}

static inline u32
idecode_batch_dequeue_soa(idecode_queue_t *q,
                          u32 *opcodes,
                          void **handles,
                          u32 count)
{
    u32 tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    u32 head = atomic_load_explicit(&q->head, memory_order_acquire);

    u32 avail = head - tail;
    if (count > avail)
        count = avail;
    if (count == 0)
        return 0;

    u32 idx   = tail & q->mask;
    u32 first = q->capacity - idx;
    if (first > count)
        first = count;

    for (u32 i = 0; i < first; i++) {
        idecode_item_t *src = &q->decoded[idx + i];
        opcodes[i] = src->opcode;
        handles[i] = src->handle;
    }

    for (u32 i = first; i < count; i++) {
        idecode_item_t *src = &q->decoded[i - first];
        opcodes[i] = src->opcode;
        handles[i] = src->handle;
    }

    atomic_store_explicit(&q->tail, tail + count, memory_order_release);
    return count;
}

#endif // __IDECODE_H__

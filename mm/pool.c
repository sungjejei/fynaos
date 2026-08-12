// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel pool manager.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/kernel.h>
#include <fynaos/mm.h>
#include <fynaos/cpu.h>

#define IS_BLOCK_USED(header)  (!!((header) & 0x8000000000000000))
#define GET_BLOCK_LEN(header)  ((header) & 0x7FFFFFFFFFFFFFFF)
#define BLOCK_USED             0x8000000000000000

struct block
{
    uint64_t header; /* SIZE and USAGE, SIZE contains its header size */
    struct block *next;
    struct block *prev;
    uint8_t data[1];
};

struct pool
{
    uint64_t len;
    uint64_t usage;
    struct pool *next;
    struct block first[];
};

struct pool *pools;
struct pool *tail;

static boolean_t divide(struct block *block, size_t data_len)
{
    if (GET_BLOCK_LEN(block->header) <= data_len + sizeof(struct block))
    {
        return FALSE;
    }

    struct block *block1 = block;
    struct block *block2 = (struct block*)((uintptr_t)block1 + data_len + sizeof(struct block));
    struct block *next   = block1->next;

    block2->header = GET_BLOCK_LEN(block->header) - (data_len + sizeof(struct block));
    block1->header = data_len + sizeof(struct block);

    block1->next   = block2;
    block2->next   = next;
    block2->prev   = block1;

    if (next)
    {
        next->prev = block2;
    }

    return TRUE;
}

static void merge(struct block *block)
{
    struct block *first  = block;
    struct block *second = block->next;

    first->header = first->header + second->header;
    first->next   = second->next;

    if (second->next)
    {
        second->next->prev = first;
    }
}

static void *allocate(struct block *first, size_t len)
{
    len = ALIGN_UP(len, 16);

    while (first)
    {
        if (!IS_BLOCK_USED(first->header) &&
            GET_BLOCK_LEN(first->header) >= len + sizeof(struct block))
        {
            break;
        }

        first = first->next;
    }

    if (!first || IS_BLOCK_USED(first->header))
    {
        return NULL;
    }

    if (GET_BLOCK_LEN(first->header) - sizeof(struct block) > len)
    {
        divide(first, len);
    }

    first->header |= BLOCK_USED;

    return first->data;
}

void kfree(void *addr)
{
    if (!addr)
    {
        return;
    }

    struct block *block = (struct block*)((uintptr_t)addr - sizeof(struct block));

    if (!IS_BLOCK_USED(block->header))
    {
        panic("Double free detected");
    }

    block->header &= ~BLOCK_USED;

    if (block->next && !IS_BLOCK_USED(block->next->header))
    {
        merge(block);
    }

    if (block->prev && !IS_BLOCK_USED(block->prev->header))
    {
        merge(block->prev);
    }
}

static void init_single_pool(struct pool *pool, size_t len)
{
    pool->len           = len;
    pool->next          = NULL;
    pool->usage         = 0;
    pool->first->header = len - sizeof(struct pool);
    pool->first->next   = NULL;
    pool->first->prev   = NULL;
}

static boolean_t extend_pool(void)
{
    phys_addr_t page = alloc_frames(0);

    if (page == INVALID_ADDRESS)
    {
        return FALSE;
    }

    zero_page(page);

    tail->next = phys_to_virt(page);
    init_single_pool(tail->next, PAGE_SIZE);
    tail = tail->next;

    return TRUE;
}

void *kmalloc(size_t len)
{
    unsigned long flags = save_and_disable_interrupts();
    void        *addr;
    struct pool *pool = pools;

    for (;;)
    {
        addr = allocate(pool->first, len);
        if (addr) break;
        if (!pool->next)
        {
            if (!extend_pool())
            {
                return NULL;
            }
        }
        pool = pool->next;
    }
    restore_interrupts(flags);
    return addr;
}

boolean_t init_pool(void)
{
    phys_addr_t page = alloc_frames(0);
    
    if (page == INVALID_ADDRESS)
    {
        return FALSE;
    }

    zero_page(page);

    pools = phys_to_virt(page);
    tail  = pools;

    init_single_pool(pools, PAGE_SIZE);
    
    return TRUE;
}

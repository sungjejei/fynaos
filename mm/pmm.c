// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Physical memory manager.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/mm.h>
#include <fynaos/kernel.h>
#include <fynaos/rtl.h>

struct frame *mm_frames;
size_t        mm_frames_count;
struct frame *mm_free_region[FRAME_MAX_ORDER + 1] = { 0 };

static page_index_t frame_to_index(struct frame *frame)
{
    return (page_index_t)(frame - mm_frames);
}

void zero_page(phys_addr_t a)
{
    memset((void*)(a + KERNEL_DIRECT_BASE), 0, PAGE_SIZE);
}

boolean_t init_frames(phys_addr_t limit)
{
    mm_frames_count = limit >> PAGE_SHIFT;
    mm_frames = alloc_boot_memory(mm_frames_count * sizeof(struct frame), 16);

    for (page_index_t i = 0; i < mm_frames_count; i++)
    {
        mm_frames[i].next  = NULL;
        mm_frames[i].flags = FRAME_RESERVED;
        mm_frames[i].order = -1;
    }

    return !!mm_frames;
}

void unreserve_frame_region(page_index_t begin, page_index_t limit)
{
    for (; begin < limit; begin++)
    {
        mm_frames[begin].flags &= (unsigned int)~FRAME_RESERVED;
    }
}

static int get_max_order(page_index_t begin, page_index_t limit)
{
    for (int order = FRAME_MAX_ORDER; order >= 0; order--)
    {
        size_t count = 1ULL << order;

        if ((begin & (count - 1)) != 0) continue;
        if (begin + count >= limit) continue;

        return order;
    }

    return 0;
}

void init_buddy_allocator(page_index_t begin, page_index_t limit)
{
    while (begin < limit)
    {
        int order = get_max_order(begin, limit);

        mm_frames[begin].order = order;
        mm_frames[begin].next = mm_free_region[order];
        mm_free_region[order] = &mm_frames[begin];

        begin += 1ULL << order;
    }
}

static void remove_frame_from_list(struct frame *frame)
{
    struct frame *cur = mm_free_region[frame->order];
    struct frame *prev = NULL;

    while (cur)
    {
        if (cur == frame) break;
        prev = cur;
        cur = cur->next;
    }

    if (!cur) panic("Trying to remove not existing frame in list");

    if (prev) prev->next = cur->next;
    else      mm_free_region[frame->order] = cur->next;
}

static void divide_buddy(int order)
{
    struct frame *frame = mm_free_region[order];
    struct frame *buddy = frame + (1ULL << (order - 1));

    mm_free_region[order] = mm_free_region[order]->next;

    frame->order = buddy->order = order - 1;
    buddy->flags = frame->flags; 

    buddy->next = mm_free_region[order - 1];
    frame->next = buddy;
    mm_free_region[order - 1] = frame;
}

static void merge_buddy(page_index_t pfn)
{
    struct frame *frame = &mm_frames[pfn];

    while (frame->order < FRAME_MAX_ORDER)
    {
        page_index_t buddy_pfn = frame_to_index(frame) ^ (1ULL << frame->order);
        if (buddy_pfn >= mm_frames_count) break;

        struct frame *buddy = &mm_frames[buddy_pfn];

        if (buddy->order != frame->order) break;
        if (buddy->flags & (FRAME_ALLOCATED | FRAME_RESERVED)) break;

        remove_frame_from_list(buddy);

        if (buddy < frame) frame = buddy;

        frame->order++;
    }

    frame->next = mm_free_region[frame->order];
    mm_free_region[frame->order] = frame;
}

phys_addr_t alloc_frames(int order)
{
    int current_order = order;

    for (; current_order < FRAME_MAX_ORDER; current_order++)
    {
        if (mm_free_region[current_order]) break;
    }

    if (!mm_free_region[current_order]) return INVALID_ADDRESS;

    for (; current_order != order; current_order--)
    {
        divide_buddy(current_order);
    }

    struct frame *frame = mm_free_region[order];
    mm_free_region[order] = mm_free_region[order]->next;
    
    frame->flags |= FRAME_ALLOCATED; 
    
    return frame_to_index(frame) << PAGE_SHIFT;
}

void free_frame(phys_addr_t addr)
{
    page_index_t pfn = addr >> PAGE_SHIFT;
    struct frame *frame = &mm_frames[pfn];
    frame->flags &= (unsigned int)~FRAME_ALLOCATED;
    merge_buddy(pfn);
}

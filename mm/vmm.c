// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Virtual memory manager.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/mm.h>
#include <fynaos/rtl.h>
#include <fynaos/cpu.h>
#include <fynaos/symbols.h>
#include <multiboot2.h>

struct mm  kernel_mm;
struct mm *current_mm = NULL;

void *phys_to_virt(phys_addr_t phys)
{
    return (void*)(phys + KERNEL_DIRECT_BASE);
}

/*
 * This function converts <virt> to a physical address.
 * <mm> is an address space for <virt>.
 */
phys_addr_t virt_to_phys(struct mm *mm, virt_addr_t virt)
{
    uintptr_t *ppml4 = phys_to_virt(mm->pml4 & PAGE_MASK);

    if (!(ppml4[PML4_INDEX(virt)] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    uintptr_t *ppdpt = phys_to_virt(ppml4[PML4_INDEX(virt)] & PAGE_MASK);

    if (!(ppdpt[PDPT_INDEX(virt)] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    uintptr_t *ppd = phys_to_virt(ppdpt[PDPT_INDEX(virt)] & PAGE_MASK);

    if (!(ppd[PD_INDEX(virt)] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    uintptr_t *ppt = phys_to_virt(ppd[PD_INDEX(virt)] & PAGE_MASK);

    if (!(ppt[PT_INDEX(virt)] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    return (ppt[PT_INDEX(virt)] & PAGE_MASK) + PAGE_OFFSET(virt);
}

/*
 * Not implemented.
 * Uses the initial page table (in boot code) instead.
 */
void init_kernel_mm(struct multiboot2_mmap_entry *entries, size_t count)
{
    (void)entries;
    (void)count;
    return; 
}

boolean_t map_page(struct mm *mm, phys_addr_t frame, virt_addr_t page, uint32_t attr)
{
    page_index_t pml4_index = PML4_INDEX(page);
    page_index_t pdpt_index = PDPT_INDEX(page);
    page_index_t pd_index   = PD_INDEX(page);
    page_index_t pt_index   = PT_INDEX(page);

    phys_addr_t *ppml4 = phys_to_virt(mm->pml4);

    if (!(ppml4[pml4_index] & PAGE_PRESENT))
    {
        zero_page(ppml4[pml4_index] = alloc_frames(0));
        ppml4[pml4_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppdpt = phys_to_virt(ppml4[pml4_index] & ~0xFFFULL);

    if (!(ppdpt[pdpt_index] & PAGE_PRESENT))
    {
        zero_page(ppdpt[pdpt_index] = alloc_frames(0));
        ppdpt[pdpt_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppd = phys_to_virt(ppdpt[pdpt_index] & ~0xFFFULL);

    if (!(ppd[pd_index] & PAGE_PRESENT))
    {
        zero_page(ppd[pd_index] = alloc_frames(0));
        ppd[pd_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppt = phys_to_virt(ppd[pd_index] & ~0xFFFULL);

    if (ppt[pt_index] & PAGE_PRESENT)
    {
        return FALSE;
    }

    ppt[pt_index] = frame | attr;

    return TRUE;
}

phys_addr_t unmap_page(struct mm *mm, virt_addr_t page)
{
    page_index_t pml4_index = PML4_INDEX(page);
    page_index_t pdpt_index = PDPT_INDEX(page);
    page_index_t pd_index   = PD_INDEX(page);
    page_index_t pt_index   = PT_INDEX(page);

    phys_addr_t *ppml4 = phys_to_virt(mm->pml4);

    if (!(ppml4[pml4_index] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    phys_addr_t *ppdpt = phys_to_virt(ppml4[pml4_index] & ~0xFFFULL);

    if (!(ppdpt[pdpt_index] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    phys_addr_t *ppd = phys_to_virt(ppdpt[pdpt_index] & ~0xFFFULL);

    if (!(ppd[pd_index] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    phys_addr_t *ppt = phys_to_virt(ppd[pd_index] & ~0xFFFULL);

    if (!(ppt[pt_index] & PAGE_PRESENT))
    {
        return INVALID_ADDRESS;
    }

    phys_addr_t phys = ppt[pt_index] & ~0xFFFULL;

    ppt[pt_index] = 0ULL;

    return phys;
}

void map_kernel_for_user_mm(struct mm *mm)
{
    phys_addr_t *user_pml4   = phys_to_virt(mm->pml4);
    phys_addr_t *kernel_pml4 = phys_to_virt(kernel_mm.pml4);

    for (int i = 256; i < 512; i++)
    {
        user_pml4[i] = kernel_pml4[i];
    }
}

struct mm *create_mm(void)
{
    struct mm *mm;
    phys_addr_t pml4_page;

    if ((pml4_page = alloc_frames(0)) == INVALID_ADDRESS)
    {
        return NULL;
    }

    if (!(mm = kmalloc(sizeof(struct mm))))
    {
        free_frame(pml4_page);
        return NULL;
    }

    zero_page(pml4_page);

    mm->pml4 = pml4_page;

    return mm;
}

void swap_mm(struct mm *mm)
{
    if (!mm || current_mm == mm) return;
    current_mm = mm;
    write_cr3(mm->pml4);
}

static void free_page_table(phys_addr_t table, int level)
{
    phys_addr_t *pt = phys_to_virt(table);
    for (int i = 0; i < 512; i++)
    {
        if (!(pt[i] & PAGE_PRESENT)) continue;
        phys_addr_t child = pt[i] & PAGE_MASK;
        if (level > 0) free_page_table(child, level - 1);
        else free_frame(child);
    }
    free_frame(table);
}

void delete_mm(struct mm *mm)
{
    free_page_table(mm->pml4, 3);
    kfree(mm);
}

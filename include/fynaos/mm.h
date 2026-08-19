#ifndef FYNAOS_MM_H
#define FYNAOS_MM_H

#include <fynaos/types.h>

#define KERNEL_IMAGE_BASE   0xFFFFFFFF80000000
#define KERNEL_DIRECT_BASE  0xFFFF800000000000
#define PAGE_SIZE           4096ULL
#define PAGE_SHIFT          12ULL
#define PAGE_MASK           (~(PAGE_SIZE - 1))

#define PML4_INDEX(addr)    (((uint64_t)(addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr)    (((uint64_t)(addr) >> 30) & 0x1FF)
#define PD_INDEX(addr  )    (((uint64_t)(addr) >> 21) & 0x1FF)
#define PT_INDEX(addr)      (((uint64_t)(addr) >> 12) & 0x1FF)
#define PAGE_OFFSET(addr)   ((uint64_t)(addr) & 0xFFF)

#define PAGE_PRESENT        (1ULL << 0)   // Present
#define PAGE_WRITABLE       (1ULL << 1)   // Read/Write
#define PAGE_USER           (1ULL << 2)   // User/Supervisor
#define PAGE_PWT            (1ULL << 3)   // Page Write Through
#define PAGE_PCD            (1ULL << 4)   // Page Cache Disable
#define PAGE_ACCESSED       (1ULL << 5)   // Accessed
#define PAGE_DIRTY          (1ULL << 6)   // Dirty (PTE only)
#define PAGE_HUGE           (1ULL << 7)
#define PAGE_PAT            (1ULL << 7)   // PAT (PTE), PS (PDE/PDPTE)
#define PAGE_GLOBAL         (1ULL << 8)   // Global (PTE only)
#define PAGE_NO_EXECUTE     (1ULL << 63)  // NX

#define FRAME_ALLOCATED     0x00000001U
#define FRAME_RESERVED      0x00000002U
#define FRAME_BAD           0x00000004U

#define FRAME_MAX_ORDER     11

struct frame {
    struct frame *next;
    unsigned int flags;
    int order;
};

struct phys_region {
    phys_addr_t addr;
    size_t len;
};

struct mm {
    uintptr_t pml4;
};

struct multiboot2_mmap_entry;

boolean_t init_frames(phys_addr_t limit);
phys_addr_t alloc_frames(int order);
void free_frame(phys_addr_t addr);
void init_buddy_allocator(page_index_t begin, page_index_t limit);
void unreserve_frame_region(page_index_t begin, page_index_t limit);
void zero_page(phys_addr_t page);

void *phys_to_virt(phys_addr_t phys);
phys_addr_t virt_to_phys(struct mm *mm, virt_addr_t virt);
boolean_t map_page(struct mm *mm, phys_addr_t frame, virt_addr_t page, uint32_t attr);
phys_addr_t unmap_page(struct mm *mm, virt_addr_t page);
void map_kernel_for_user_mm(struct mm *mm);
struct mm *create_mm(void);
void swap_mm(struct mm *mm);
void init_kernel_mm(struct multiboot2_mmap_entry *entries, size_t count);
void delete_mm(struct mm *mm);

void *kmalloc(size_t len);
void kfree(void *addr);
boolean_t init_pool(void);

void init_memory(struct multiboot2_mmap_entry *entries, size_t count);

#endif

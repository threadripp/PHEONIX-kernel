#include "kernel.h"

static uint8_t pmm_bitmap[BITMAP_SIZE];

static void bitmap_set(uint64_t bit) {
    pmm_bitmap[bit / 8] |= (1 << (bit % 8));
}

static void bitmap_clear(uint64_t bit) {
    pmm_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static uint8_t bitmap_test(uint64_t bit) {
    return (pmm_bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

void pmm_init(void) {
    for (int i = 0; i < BITMAP_SIZE; i++) {
        pmm_bitmap[i] = 0; // Mark all frames as free
    }
    // Mark first 2MB (Kernel space) as allocated
    for (uint64_t i = 0; i < 512; i++) {
        bitmap_set(i);
    }
    serial_print("[PMM] Physical Frame Allocator initialized (128MB mapped).\n");
}

void* pmm_alloc_frame(void) {
    for (uint64_t i = 0; i < BITMAP_SIZE * 8; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            return (void*)(i * PAGE_SIZE);
        }
    }
    return 0; // Out of memory
}

void pmm_free_frame(void* frame) {
    uint64_t bit = (uint64_t)frame / PAGE_SIZE;
    bitmap_clear(bit);
}

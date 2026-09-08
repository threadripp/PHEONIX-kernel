#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768 // Manages up to 128MB physical memory

// IDT Structures
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct registers {
    uint64_t r11, r10, r9, r8, rdx, rcx, rbx, rax;
    uint64_t int_no, error_code;
    uint64_t rip, cs, rflags, rsp, ss;
};

// GDT & TSS Structures
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct tss_entry {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Kernel Function Declarations
void serial_init(void);
void serial_print(const char* str);
void pmm_init(void);
void* pmm_alloc_frame(void);
void pmm_free_frame(void* frame);
void gdt_tss_init(void);
void idt_init(void);
void syscall_init(void);

#endif

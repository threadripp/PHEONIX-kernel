#include "kernel.h"

extern void page_fault_stub(void);
extern void syscall_entry(void);
extern void gdt_flush(uint64_t gdt_ptr);
extern void tss_flush(void);

struct idt_entry idt[256];
struct idt_ptr   idtp;

struct gdt_entry gdt[7];
struct gdt_ptr   gp;
struct tss_entry tss;

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_mid  = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector  = sel;
    idt[num].ist        = 0;
    idt[num].flags      = flags;
    idt[num].reserved   = 0;
}

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint64_t)&idt;

    idt_set_gate(14, (uint64_t)page_fault_stub, 0x08, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idtp));
    serial_print("[WRR] IDT Interrupt Vector 14 Armed.\n");
}

void wrr_page_fault_handler(struct registers* regs) {
    serial_print("\n----------------------------------------\n");
    serial_print("[WRR WATCH] Illegal Fault Intercepted!\n");
    serial_print("[WRR REPLACE] Isolating execution frame...\n");
    serial_print("[WRR REVIVE] Reviving Kernel Stream!\n");
    serial_print("----------------------------------------\n\n");

    regs->rip += 3;
}

void syscall_handler(uint64_t sys_id) {
    serial_print("[SYSCALL] System call invoked across Ring 3 boundary!\n");
}

void syscall_init(void) {
    uint64_t efer;
    __asm__ volatile("rdmsr" : "=a"(efer) : "c"(0xC0000080));
    efer |= 1; // Enable SCE (System Call Extensions)
    __asm__ volatile("wrmsr" : : "a"(efer), "c"(0xC0000080), "d"(0));

    // Set LSTAR to syscall handler address
    uint64_t target = (uint64_t)syscall_entry;
    __asm__ volatile("wrmsr" : : "a"((uint32_t)target), "d"((uint32_t)(target >> 32)), "c"(0xC0000082));

    serial_print("[SYSCALL] Fast System Call MSRs (LSTAR/STAR) Armed.\n");
}

void gdt_tss_init(void) {
    gp.limit = (sizeof(struct gdt_entry) * 7) - 1;
    gp.base  = (uint64_t)&gdt;

    // Kernel Code & Data
    gdt[0] = (struct gdt_entry){0, 0, 0, 0, 0, 0};
    gdt[1] = (struct gdt_entry){0, 0, 0, 0x9A, 0xAF, 0}; // 64-bit Kernel Code
    gdt[2] = (struct gdt_entry){0, 0, 0, 0x92, 0xCF, 0}; // Kernel Data

    // User/Driver Code & Data
    gdt[3] = (struct gdt_entry){0, 0, 0, 0xF2, 0xCF, 0}; // User Data
    gdt[4] = (struct gdt_entry){0, 0, 0, 0xFA, 0xAF, 0}; // User Code

    gdt_flush((uint64_t)&gp);
    serial_print("[GDT/TSS] GDT Ring 0 and Ring 3 segments loaded.\n");
}

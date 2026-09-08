#include "idt.h"
#include <stdint.h>

extern void page_fault_stub(void);
extern void serial_print(const char* str);

struct idt_entry idt[256];
struct idt_ptr   idtp;

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
    serial_print("[WRR] IDT Interrupt Vector 14 Trap Armed.\n");
}

void wrr_page_fault_handler(struct registers* regs) {
    serial_print("\n----------------------------------------\n");
    serial_print("[WRR WATCH] Illegal Access Trapped!\n");
    serial_print("[WRR REPLACE] Isolating execution context...\n");
    serial_print("[WRR REVIVE] Reviving execution stream!\n");
    serial_print("----------------------------------------\n\n");

    regs->rip += 3;
}

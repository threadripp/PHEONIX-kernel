#include "kernel.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
}

void serial_putc(char c) {
    outb(0x3F8, c);
}

void serial_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        serial_putc(str[i]);
    }
}

void kernel_main(void) {
    serial_init();
    
    serial_print("\n==================================================\n");
    serial_print("   PHEONIX Kernel v0.1.0 [Complete Architecture]\n");
    serial_print("==================================================\n");
    
    pmm_init();
    gdt_tss_init();
    idt_init();
    syscall_init();

    void* test_frame = pmm_alloc_frame();
    if (test_frame) {
        serial_print("[PMM TEST] Allocated physical frame successfully.\n");
    }

    serial_print("[INIT] All core subsystems active. Main kernel loop running.\n\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}

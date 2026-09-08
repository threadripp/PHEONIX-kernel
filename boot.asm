bits 32
section .multiboot
    align 4
    dd 0x1BADB002               ; Multiboot magic
    dd 0x03                     ; Flags: ALIGN + MEMINFO
    dd -(0x1BADB002 + 0x03)     ; Checksum

section .bss
align 4096
p4_table:     resb 4096
p3_table:     resb 4096
p2_table:     resb 4096
stack_bottom: resb 16384
stack_top:

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top

    ; Identity Map 0MB - 2MB in Paging Tables
    mov eax, p3_table
    or eax, 0b11
    mov [p4_table], eax

    mov eax, p2_table
    or eax, 0b11
    mov [p3_table], eax

    mov dword [p2_table], 0b10000011 ; 2MB Page + Present + Writable

    mov eax, p4_table
    mov cr3, eax

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Enable Long Mode in EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable Paging
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)
    mov cr0, eax

    lgdt [gdt64.pointer]
    jmp gdt64.code_segment:long_mode_start

bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call kernel_main

.halt:
    hlt
    jmp .halt

section .rodata
gdt64:
    dq 0
.code_segment: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

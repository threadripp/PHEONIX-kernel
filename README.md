[Video of booting PHEONIX] https://github.com/user-attachments/assets/7efaa844-9d90-4477-832d-97f29efd1bc3

# PHEONIX-kernel
a new type of open source kernel will rise from the dead, PHEONIX is an open source kernel inspired by linux and based off of XNU, MacOS kernel.

PHEONIX is a bare-metal, non-Unix x86_64 hybrid kernel written from scratch in C99 and NASM assembly. Designed for high-reliability systems, PHEONIX features a **Watch, Replace, Revive (WRR)** fault isolation model that intercepts hardware exceptions (such as `#PF` Page Faults) to recover execution state without causing kernel panics or CPU triple faults.

+-------------------------------------------------------------------+
|                         PHEONIX KERNEL                            |
+-------------------------------------------------------------------+
|  [ Physical Frame Allocator ]    <-->    [ Dynamic Paging PML4 ]  |
|  [ Ring 0 / Ring 3 GDT + TSS ]   <-->    [ SYSCALL / SYSRET Gate ]|
+-------------------------------------------------------------------+
|
+-----------------------+
| WRR Isolation Engine  |
|   (Vector 14 / #PF)   |
+-----------------------+

---

## Architectural Features

* **Execution Environment:** Full 64-Bit Long Mode transitioned directly from a 32-bit Multiboot header.
* **WRR Driver Isolation:** Custom exception trap for Vector 14 (`#PF`) that handles invalid memory access, isolates driver context, and revives the thread stream.
* **Physical Memory Manager (PMM):** High-efficiency 4 KiB bitmap frame allocator capable of managing physical memory dynamically.
* **GDT & TSS Security Domains:** Configured privilege rings supporting Ring 0 (Kernel) and Ring 3 (User / Isolated Drivers) execution segments.
* **Fast System Call Gate:** Direct `SYSCALL`/`SYSRET` MSR registers (`IA32_LSTAR`, `IA32_STAR`) for low-overhead Ring 3 to Ring 0 control transfers.
* **Headless Serial Output:** Integrated COM1 (`0x3F8`) UART serial logging engine for real-time telemetry across hypervisors (UTM, QEMU, bare metal).

---

## Repository Structure

```text
.
├── arch/
│   └── x86_64/
│       ├── boot.asm          # 32-to-64 bit Multiboot initialization
│       └── kernel_stubs.asm  # IDT, GDT/TSS flushes, and SYSCALL assembly stubs
├── kernel/
│   ├── kernel.h              # Central kernel structures, constants, and API gates
│   ├── main.c                # Kernel entry point and boot orchestration
│   ├── pmm.c                 # Physical Memory Bitmap Allocator
│   └── subsystems.c          # GDT, TSS, IDT, WRR Engine, & Syscall MSR setup
├── linker.ld                 # Kernel memory placement layout
├── Makefile                  # Automated build and ISO generation pipeline
└── README.md                 # Project documentation
Getting Started
Prerequisites
To compile PHEONIX, you need an x86_64 ELF cross-compiler toolchain:
x86_64-elf-gcc
nasm
x86_64-elf-ld
grub-mkrescue (requires xorriso / mtools)
Building the Kernel ISO
Clone the repository:
Bash
git clone [https://github.com/YOUR_USERNAME/pheonix.git](https://github.com/YOUR_USERNAME/pheonix.git)
cd pheonix
Compile source code and generate pheonix.iso:
Bash
make clean && make ARCH=64
Run in QEMU / UTM with Serial Terminal Output:
Bash
qemu-system-x86_64 -cdrom pheonix.iso -serial stdio
License
PHEONIX is licensed under the MIT License. Contributions and architectural forks are welcome!
EOF

---

### Push to GitHub

Once created, stage and commit the README to your repository:

```bash
git add README.md
git commit -m "docs: add comprehensive README with architecture map and build instructions"
git push

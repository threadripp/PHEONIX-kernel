ARCH ?= 64

ifeq ($(ARCH),64)
	CC = x86_64-elf-gcc
	LD = x86_64-elf-ld
	NASM_FLAGS = -f elf64
	GCC_FLAGS = -std=c99 -ffreestanding -mcmodel=large -mno-red-zone -O2
	LD_FLAGS = -n
	BOOT_SRC = arch/x86_64/boot.asm
endif

all: pheonix.iso

boot.o: $(BOOT_SRC)
	nasm $(NASM_FLAGS) $(BOOT_SRC) -o boot.o

kernel_stubs.o: arch/x86_64/kernel_stubs.asm
	nasm $(NASM_FLAGS) arch/x86_64/kernel_stubs.asm -o kernel_stubs.o

pmm.o: kernel/pmm.c
	$(CC) $(GCC_FLAGS) -c kernel/pmm.c -o pmm.o

subsystems.o: kernel/subsystems.c
	$(CC) $(GCC_FLAGS) -c kernel/subsystems.c -o subsystems.o

kernel.o: kernel/main.c
	$(CC) $(GCC_FLAGS) -c kernel/main.c -o kernel.o

pheonix.bin: boot.o kernel_stubs.o pmm.o subsystems.o kernel.o
	$(LD) $(LD_FLAGS) -n --no-warn-rwx-segments -T linker.ld -o pheonix.bin boot.o kernel_stubs.o pmm.o subsystems.o kernel.o

pheonix.iso: pheonix.bin
	mkdir -p iso/boot/grub
	cp pheonix.bin iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "PHEONIX Kernel" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/pheonix.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	x86_64-elf-grub-mkrescue -o pheonix.iso iso

clean:
	rm -rf *.o *.bin *.iso iso

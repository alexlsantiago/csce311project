# RISC-V OS Makefile
ARCH = riscv64
CROSS_COMPILE = riscv64-unknown-elf-

CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS = -march=rv64imafdc -mabi=lp64d -mcmodel=medany \
         -Wall -Wextra -O2 -g -ffreestanding -nostdlib \
         -fno-common -fno-builtin -fno-stack-protector \
         -Iinclude -DKERNEL

ASFLAGS = -march=rv64imafdc -mabi=lp64d

LDFLAGS = -T linker.ld -nostdlib -static

KERNEL_SRCS = kernel/main.c \
              kernel/init.c \
              kernel/console.c \
              kernel/memory.c \
              kernel/paging.c \
              kernel/task.c \
              kernel/scheduler.c \
              kernel/sync.c \
              kernel/syscall.c \
              kernel/fs.c \
              kernel/elf.c \
              kernel/shell.c \
              kernel/string.c \
              kernel/printf.c \
              drivers/uart.c \
              drivers/virtio.c

KERNEL_ASM = boot/entry.S

KERNEL_OBJS = $(KERNEL_SRCS:.c=.o) $(KERNEL_ASM:.S=.o)

KERNEL = kernel.elf
KERNEL_BIN = kernel.bin

.PHONY: all clean run qemu

all: $(KERNEL_BIN)

$(KERNEL_BIN): $(KERNEL)
	$(OBJCOPY) -O binary $< $@

$(KERNEL): $(KERNEL_OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL_OBJS) $(KERNEL) $(KERNEL_BIN)

run: $(KERNEL_BIN) disk.img
	qemu-system-riscv64 -machine virt -cpu rv64 -m 128M \
		-nographic -bios default -kernel $(KERNEL_BIN) \
		-drive file=disk.img,format=raw,id=hd0 \
		-device virtio-blk-device,drive=hd0

qemu: run

disk:
	dd if=/dev/zero of=disk.img bs=1M count=10


#!/bin/bash
# Quick run script for RISC-V OS

# Check if kernel.bin exists
if [ ! -f kernel.bin ]; then
    echo "Building kernel..."
    make
fi

# Check if disk.img exists
if [ ! -f disk.img ]; then
    echo "Creating disk image..."
    make disk
fi

# Run QEMU
echo "Starting QEMU..."
qemu-system-riscv64 -machine virt -cpu rv64 -m 1024M \
    -nographic -bios default -kernel kernel.bin \
    -drive file=disk.img,format=raw,id=hd0 \
    -device virtio-blk-device,drive=hd0


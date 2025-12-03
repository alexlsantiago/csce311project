# RISC-V Operating System

A working command-line OS built from scratch for RISC-V architecture using QEMU.

This project implements a minimal but functional 64-bit RISC-V operating system, including:

Bare-metal UART driver

Custom printf

Memory manager

Simple task scheduler

Task forking

File system support

Timer ticks + uptime

An interactive shell

Boot flow integrated with OpenSBI
## Quick Start

```bash
# Install prerequisites (example for macOS)
brew install riscv-gnu-toolchain qemu

# Build and run
make
make run
```
=== RISC-V OS Boot ===
Initializing memory...
Initializing timer...
Initializing file system...
Initializing task system...
Starting shell...

RISC-V OS Shell v1.0
Type 'help' for commands
>

The OS will boot and you'll see an interactive shell. Type `help` to see available commands.

## Features
Bootloader / Kernel Entry
-BSS clearing
-Stack initialization
-Jump into kernel_main()
UART Driver
PRINTF
Timer + Uptime
Memory Manager
Simple Task Scheduler
File System
Interactive Shell

### Core Features
- **Bootloader**: RISC-V entry point with BSS clearing and stack setup
- **Memory Management**: 
  - Heap allocation (kmalloc/kfree)
  - Page allocation
  - Memory protection framework
- **Multitasking**: 
  - Task creation and management
  - Round-robin scheduler
  - Process forking
  - Process waiting
- **Synchronization**:
  - Spinlocks
  - Semaphores
  - Mutexes
- **File System**: 
  - Custom file system (OSFS)
  - File creation, reading, and writing
  - File listing
- **Program Loading**: 
  - ELF loader for RISC-V executables
  - Program execution
- **Shell Interface**: 
  - Interactive command-line shell
  - Built-in commands (help, ls, cat, echo, ps, fork, exit)

### System Calls
- `SYS_EXIT` - Exit process
- `SYS_WRITE` - Write to stdout/stderr
- `SYS_READ` - Read from stdin
- `SYS_FORK` - Fork current process
- `SYS_EXEC` - Execute program
- `SYS_WAIT` - Wait for child process
- `SYS_OPEN/CLOSE` - File operations
- `SYS_READ_FS/SYS_WRITE_FS` - File system operations

## Project Structure

```
csce311project/
├── boot/
│   ├── entry.S          # Boot entry + jump to kernel_main
│   └── trap.S           # Trap stub (not fully used yet)
│
├── kernel/
│   ├── main.c           # Kernel initialization
│   ├── console.c        # Console / screen helpers
│   ├── memory.c         # Memory allocator + counters
│   ├── paging.c         # Page table setup
│   ├── timer.c          # 64-bit tick counter
│   ├── trap.c           # Trap handler (basic)
│   ├── task.c           # Task creation + fork
│   ├── scheduler.c      # Ready queue + task list
│   ├── fs.c             # Simple embedded file system
│   ├── elf.c            # (Stub) ELF loader
│   ├── shell.c          # Interactive shell
│   ├── printf.c         # Custom printf
│   └── string.c         # Basic libc-like utilities
│
├── drivers/
│   ├── uart.c           # UART driver
│   └── virtio.c         # VirtIO block device
│
├── include/
│   ├── *.h              # All kernel headers
│
├── disk.img             # File system disk image
├── linker.ld            # Kernel memory layout
├── Makefile             # Build system
└── run.sh               # QEMU launcher


## Building

### Prerequisites

You need a RISC-V cross-compiler toolchain:

```bash
# On Ubuntu/Debian
sudo apt-get install gcc-riscv64-unknown-elf

# On macOS (using Homebrew)
brew install riscv-gnu-toolchain

# Or build from source
# https://github.com/riscv/riscv-gnu-toolchain
```

You also need QEMU with RISC-V support:

```bash
# On Ubuntu/Debian
sudo apt-get install qemu-system-riscv64

# On macOS
brew install riscv-gnu-toolchain qemu

```

### Build Commands

```bash
# Build the kernel
make

# Create a disk image (10MB)
make disk

# Run in QEMU
make run
# or
make qemu
```

### Manual QEMU Command

If you need to run QEMU manually:

```bash
qemu-system-riscv64 -machine virt -cpu rv64 -m 128M \
    -nographic -bios default -kernel kernel.bin \
    -drive file=disk.img,format=raw,id=hd0 \
    -device virtio-blk-device,drive=hd0
```

## Running

1. Build the kernel: `make`
2. Create disk image: `make disk` (first time only)
3. Run: `make run`

The OS will boot and display:
```
=== RISC-V OS Boot ===
Initializing memory...
Initializing file system...
Initializing task system...
Starting shell...

RISC-V OS Shell v1.0
Type 'help' for commands

$ 
```

## Shell Commands

- `help` - Show available commands
- `ls` - List files in the file system
- `cat <file>` - Display file contents
- `echo <text>` - Echo text to console
- `uptime` - Show timer ticks
- `ps` - List running processes
- `meminfo` - Show memory usage
- `fork` - Fork the current process
- `exit` - Exit the shell

## Architecture Details

### Memory Layout
- Kernel loaded at: `0x80200000`
- Heap: `0x80400000` - `0x88000000`
- Page pool: `0x90000000` - `0x98000000`
- File system: `0xA0000000`
- Stack: Defined in linker script

### Task Management
- Maximum tasks: 32
- Task states: RUNNING, READY, BLOCKED, ZOMBIE
- Round-robin scheduling
- Each task has its own stack and page table

### File System
- Custom OSFS format
- Block size: 512 bytes
- Maximum files: 64
- Maximum filename length: 256 characters
- Superblock contains file metadata and block bitmap

## Implementation Notes

### What's Implemented
- ✅ Basic bootloader
- ✅ Memory management (heap and pages)
- ✅ Task creation and scheduling
- ✅ Synchronization primitives
- ✅ File system with read/write
- ✅ ELF program loader
- ✅ Interactive shell
- ✅ System call interface

### Limitations and Future Work
- Context switching is simplified (no actual register save/restore)
- Paging is simplified (identity mapping)
- No interrupt handling
- No device drivers beyond UART
- File system is in-memory only
- No user/kernel mode separation
- Limited error handling

### For Production Use
To make this production-ready, you would need to:
1. Implement proper RISC-V Sv39 paging
2. Add interrupt and exception handling
3. Implement full context switching with register save/restore
4. Add proper device drivers (VirtIO, etc.)
5. Implement persistent storage
6. Add user/kernel mode separation
7. Add more robust error handling
8. Implement proper process isolation

## Development Log

This OS was developed as a learning project to understand operating system internals. Key decisions:

- **Simplified paging**: Used identity mapping for simplicity
- **Round-robin scheduling**: Simple and fair
- **In-memory file system**: Easy to implement and test
- **UART-only I/O**: Simplest output method for QEMU

## License

This project is for educational purposes.

## References

- [RISC-V Instruction Set Manual](https://riscv.org/technical/specifications/)
- [QEMU RISC-V Documentation](https://www.qemu.org/docs/master/system/riscv/virt.html)
- [ELF Format Specification](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)

# RISC-V Operating System

A working command-line operating system for RISC-V architecture, built from scratch. This OS includes multitasking, synchronization primitives, a file system, program loading, and a shell interface.

## Quick Start

```bash
# Install prerequisites (example for macOS)
brew install riscv-gnu-toolchain qemu

# Build and run
make
make run
```

The OS will boot and you'll see an interactive shell. Type `help` to see available commands.

## Features

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
│   └── entry.S          # Bootloader entry point
├── kernel/
│   ├── main.c           # Kernel initialization
│   ├── init.c           # Initialization helpers
│   ├── console.c        # Console interface
│   ├── memory.c         # Memory management
│   ├── paging.c         # Page table management
│   ├── task.c           # Task/process management
│   ├── scheduler.c      # Task scheduler
│   ├── sync.c           # Synchronization primitives
│   ├── syscall.c        # System call handler
│   ├── fs.c             # File system implementation
│   ├── elf.c            # ELF loader
│   ├── shell.c          # Shell implementation
│   ├── string.c         # String utilities
│   └── printf.c         # Printf implementation
├── drivers/
│   ├── uart.c           # UART driver
│   └── virtio.c         # VirtIO block device driver
├── include/
│   ├── types.h          # Type definitions
│   ├── kernel.h         # Kernel constants and declarations
│   ├── memory.h         # Memory management API
│   ├── task.h           # Task management API
│   ├── scheduler.h      # Scheduler API
│   ├── sync.h           # Synchronization API
│   ├── fs.h             # File system API
│   ├── elf.h            # ELF structures
│   ├── uart.h           # UART API
│   ├── paging.h         # Paging API
│   ├── shell.h          # Shell API
│   └── string.h         # String API
├── Makefile             # Build system
├── linker.ld            # Linker script
└── README.md            # This file
```

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
brew install qemu
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
- `ps` - List running processes
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

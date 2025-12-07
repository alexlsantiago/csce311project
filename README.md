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

# IMPORTANT:
# Some environments have issues with `make run`
# If that happens, use the working launcher:
./run.sh
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

## Chat Logs With AI

Q&A Highlights from Project Development

Team Member: How do we start a minimal RISC-V OS?
AI: Begin with a bootloader (entry.S) to set up the stack and clear BSS. Then jump to a kernel_main() in kernel/main.c. Use UART to print a simple message to confirm the kernel boots.

Team Member: How do we add a shell?
AI: Create kernel/shell.c and include/shell.h. Implement a loop that reads input from UART and handles basic commands like help and echo. Integrate the shell in kernel_main() after kernel initialization.

Team Member: We need memory management. How do we do that?
AI: Add kernel/memory.c and include/memory.h. Implement kmalloc and kfree for heap allocation. Also include kernel/string.c and kernel/printf.c for basic utilities. These will be used by the shell and kernel.

Team Member: How can we support multiple tasks?
AI: Implement kernel/task.c and include/task.h. Define task structures, states (RUNNING, READY, BLOCKED), and task creation functions. Add kernel/scheduler.c for a round-robin scheduler. Use task_yield() to switch tasks cooperatively.

Team Member: Can we run separate programs?
AI: Yes. Add kernel/elf.c and include/elf.h to load ELF executables. Combine with kernel/fs.c/include/fs.h for a simple in-memory file system. Add a shell command like exec <program> and a system call SYS_EXEC for running new programs.

Team Member: How do we safely share resources between tasks?
AI: Implement kernel/sync.c and include/sync.h. Add mutexes, semaphores, and spinlocks. Use task_wait() for tasks that block while waiting on a resource. This prevents race conditions.

Team Member: Can we track uptime or implement timers?
AI: Add kernel/timer.c and include/timer.h. Maintain a 64-bit tick counter. Add a shell command uptime to display the counter. This can be used for scheduling or performance metrics.

Team Member: What’s next after multitasking and ELF loading?
AI: You could add:
File system persistence
Paging with Sv39 for memory protection
Interrupt handling
More shell commands (ps, meminfo, etc.)
Device drivers beyond UART

Team Member: How can we implement basic synchronization for multiple programs accessing files?
AI: Use sync.c mutexes or semaphores to guard file system operations. Wrap file read/write calls with lock/unlock.

Team Member: What about running multiple programs simultaneously?
AI: Use the task scheduler. Each program gets its own task_t structure and stack. ELF loader can load multiple programs into memory. Scheduler switches between them round-robin.

Team Member: Can we implement simple protection for each process?
AI: Yes. Use separate page tables (satp) per task. Add identity mapping for kernel, separate mappings for user programs. Later, you can extend to Sv39 paging.

Team Member: What’s the first feature to add after a basic shell?
AI: A simple timer and uptime command, then multitasking with a basic round-robin scheduler. Next, ELF loading and the file system.

Team Member: How do we add a command to list running processes?
AI: Implement a ps shell command. Iterate over the task list in scheduler.c and print PID, state, and task name.

Team Member: Can we add memory usage reporting?
AI: Add a meminfo shell command. Use kernel/memory.c to report total heap size, used memory, and free memory.

Team Member: How do we handle file creation and reading in the shell?
AI: Use fs.c and fs.h. Add shell commands like touch <file> and cat <file> to create and read files. Connect to system calls SYS_READ_FS and SYS_WRITE_FS.

Some chat logs were lost due to technical issues.
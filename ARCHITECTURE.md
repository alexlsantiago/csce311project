# RISC-V OS Architecture Documentation

## Overview

This document describes the architecture and design decisions of the RISC-V Operating System.

## Boot Process

1. **Entry Point** (`boot/entry.S`):
   - Sets up stack pointer
   - Clears BSS section
   - Jumps to `kernel_init()`

2. **Kernel Initialization** (`kernel/main.c`):
   - Initializes UART for console output
   - Initializes memory management
   - Initializes file system
   - Initializes task system and scheduler
   - Starts interactive shell

## Memory Management

### Heap Allocator
- **Location**: `kernel/memory.c`
- **Algorithm**: First-fit with block splitting and merging
- **Features**:
  - Thread-safe (uses spinlocks)
  - Block headers track size and free status
  - Automatic coalescing of adjacent free blocks

### Page Allocator
- Simple page pool allocator
- Allocates 4KB pages
- Used for task stacks and page tables

### Memory Layout
```
0x80200000 - Kernel code
0x80400000 - Heap start
0x88000000 - Heap end
0x90000000 - Page pool start
0x98000000 - Page pool end
0xA0000000 - File system
```

## Task Management

### Task Structure
Each task contains:
- 32 RISC-V registers
- Program counter and stack pointer
- Page table pointer
- State (RUNNING, READY, BLOCKED, ZOMBIE)
- Process ID and parent PID
- Stack memory
- Wait queue for synchronization

### Scheduler
- **Algorithm**: Round-robin
- **Implementation**: `kernel/scheduler.c`
- Maintains ready queue of tasks
- Yields control between tasks

### Process Operations
- **Fork**: Creates copy of current process
- **Exec**: Loads and executes ELF program
- **Wait**: Waits for child process to exit
- **Exit**: Terminates current process

## Synchronization

### Spinlocks
- Atomic test-and-set operations
- Used for short critical sections
- Busy-waiting

### Semaphores
- Counter-based synchronization
- Wait/signal operations
- Thread-safe

### Mutexes
- Mutual exclusion locks
- Support for recursive locking
- Wait queue for blocked tasks

## File System

### OSFS Format
- **Superblock**: Contains magic number, file count, file entries, block bitmap
- **File Entries**: Name, size, start block, number of blocks
- **Block Size**: 512 bytes
- **Maximum Files**: 64
- **Maximum Filename**: 256 characters

### Operations
- Create file
- Read file
- Write file
- List files

### Implementation Notes
- Currently in-memory only
- Block allocation uses bitmap
- Files are stored contiguously

## Program Loading

### ELF Loader
- **Location**: `kernel/elf.c`
- Supports RISC-V ELF format
- Loads program segments
- Sets up entry point
- Handles BSS initialization

### Process
1. Read ELF header
2. Verify magic number and architecture
3. Load each program segment
4. Zero BSS sections
5. Set program counter to entry point

## System Calls

### Interface
System calls are handled in `kernel/syscall.c`:
- `SYS_EXIT`: Terminate process
- `SYS_WRITE`: Write to stdout/stderr
- `SYS_READ`: Read from stdin
- `SYS_FORK`: Create child process
- `SYS_EXEC`: Execute program
- `SYS_WAIT`: Wait for child
- `SYS_READ_FS/SYS_WRITE_FS`: File operations

## Shell

### Implementation
- **Location**: `kernel/shell.c`
- Interactive command-line interface
- Command parsing and execution
- Built-in commands

### Commands
- `help`: Show help
- `ls`: List files
- `cat <file>`: Display file
- `echo <text>`: Echo text
- `ps`: List processes
- `fork`: Fork process
- `exit`: Exit shell

## Device Drivers

### UART
- **Location**: `drivers/uart.c`
- QEMU virt machine UART at `0x10000000`
- Polling-based I/O
- Handles newline/carriage return

### VirtIO
- **Location**: `drivers/virtio.c`
- Placeholder for block device
- Would handle disk I/O in full implementation

## Design Decisions

### Simplifications
1. **Paging**: Identity mapping instead of full Sv39 paging
2. **Context Switching**: Simplified without full register save/restore
3. **Interrupts**: Not implemented
4. **User Mode**: All code runs in supervisor mode
5. **File System**: In-memory only, no persistence

### Why These Choices?
- **Educational Focus**: Keep complexity manageable
- **Time Constraints**: Full implementation would require significantly more code
- **Testing**: Easier to test and debug simplified versions
- **Learning**: Understand core concepts before adding complexity

## Future Enhancements

1. **Full Paging**: Implement RISC-V Sv39 paging
2. **Interrupt Handling**: Add interrupt controller support
3. **Context Switching**: Full register save/restore
4. **User Mode**: Separate user and kernel spaces
5. **Persistent Storage**: Real disk I/O
6. **More Drivers**: Network, graphics, etc.
7. **Process Isolation**: Memory protection between processes
8. **Virtual Memory**: Demand paging, swapping

## Testing

### Manual Testing
- Boot and verify shell appears
- Test each shell command
- Create and read files
- Fork processes
- Test memory allocation

### Known Limitations
- No error recovery
- Limited error messages
- No input validation
- Simplified synchronization
- No preemption (cooperative multitasking)

## Performance Considerations

- **Heap**: O(n) allocation time where n is number of blocks
- **Scheduler**: O(1) task selection
- **File System**: O(n) file lookup where n is number of files
- **Memory**: No fragmentation handling beyond basic coalescing

## Security Considerations

- No user/kernel separation
- No memory protection
- No input sanitization
- No access control
- All processes run with full privileges

This is expected for an educational OS and would need significant work for production use.


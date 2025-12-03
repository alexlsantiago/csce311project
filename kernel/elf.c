#include "elf.h"
#include "fs.h"
#include "memory.h"
#include "string.h"
#include "types.h"

int elf_load(const char* path, uint64_t* entry) {
    Elf64_Ehdr ehdr;
    
    /* Read ELF header */
    if (fs_read_file(path, &ehdr, sizeof(ehdr), 0) != sizeof(ehdr)) {
        return -1;
    }
    
    /* Check ELF magic */
    if (ehdr.e_ident[0] != 0x7f || 
        ehdr.e_ident[1] != 'E' ||
        ehdr.e_ident[2] != 'L' ||
        ehdr.e_ident[3] != 'F') {
        return -1;
    }
    
    /* Check architecture */
    if (ehdr.e_machine != 0xF3) {  /* RISC-V */
        return -1;
    }
    
    *entry = ehdr.e_entry;
    
    /* Load program segments */
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf64_Phdr phdr;
        uint64_t phdr_offset = ehdr.e_phoff + i * ehdr.e_phentsize;
        
        if (fs_read_file(path, &phdr, sizeof(phdr), phdr_offset) != sizeof(phdr)) {
            continue;
        }
        
        if (phdr.p_type == PT_LOAD) {
            /* Allocate memory */
            void* vaddr = (void*)phdr.p_vaddr;
            
            /* Read segment data */
            uint8_t* data = kmalloc(phdr.p_filesz);
            if (!data) {
                return -1;
            }
            
            if (fs_read_file(path, data, phdr.p_filesz, phdr.p_offset) != (int)phdr.p_filesz) {
                kfree(data);
                return -1;
            }
            
            /* Copy to destination */
            memcpy(vaddr, data, phdr.p_filesz);
            
            /* Zero BSS */
            if (phdr.p_memsz > phdr.p_filesz) {
                memset((char*)vaddr + phdr.p_filesz, 0, phdr.p_memsz - phdr.p_filesz);
            }
            
            kfree(data);
        }
    }
    
    return 0;
}


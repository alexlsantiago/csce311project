#include "fs.h"
#include "memory.h"
#include "string.h"
#include "types.h"
#include "kernel.h"
#include "drivers/virtio.h"

static fs_superblock_t* superblock = NULL;
static void* fs_base = NULL;

int fs_init(void) {
    /* Try to read superblock from disk */
    fs_base = (void*)0xA0000000;  /* Memory-mapped disk area */
    superblock = (fs_superblock_t*)fs_base;
    
    /* Check if filesystem exists */
    if (superblock->magic != FS_MAGIC) {
        /* Initialize new filesystem */
        memset(superblock, 0, sizeof(fs_superblock_t));
        superblock->magic = FS_MAGIC;
        superblock->num_files = 0;
        memset(superblock->block_bitmap, 0, sizeof(superblock->block_bitmap));
        
        /* Mark superblock blocks as used */
        int superblock_blocks = (sizeof(fs_superblock_t) + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for (int i = 0; i < superblock_blocks; i++) {
            superblock->block_bitmap[i / 8] |= (1 << (i % 8));
        }
    }
    
    return 0;
}

static int find_free_blocks(int num_blocks) {
    if (!superblock) return -1;
    
    for (int i = 0; i < FS_BLOCKS - num_blocks; i++) {
        int found = 1;
        for (int j = 0; j < num_blocks; j++) {
            int block = i + j;
            if (superblock->block_bitmap[block / 8] & (1 << (block % 8))) {
                found = 0;
                break;
            }
        }
        if (found) {
            /* Mark blocks as used */
            for (int j = 0; j < num_blocks; j++) {
                int block = i + j;
                superblock->block_bitmap[block / 8] |= (1 << (block % 8));
            }
            return i;
        }
    }
    return -1;
}

int fs_create_file(const char* name, uint32_t size) {
    if (!superblock || superblock->num_files >= MAX_FILES) {
        return -1;
    }
    
    /* Check if file already exists */
    if (fs_find_file(name)) {
        return -1;
    }
    
    int num_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int start_block = find_free_blocks(num_blocks);
    if (start_block < 0) {
        return -1;
    }
    
    file_entry_t* entry = &superblock->files[superblock->num_files++];
    strncpy(entry->name, name, MAX_FILENAME - 1);
    entry->size = size;
    entry->start_block = start_block;
    entry->blocks = num_blocks;
    entry->type = 0;
    
    return 0;
}

file_entry_t* fs_find_file(const char* name) {
    if (!superblock) return NULL;
    
    for (int i = 0; i < superblock->num_files; i++) {
        if (strcmp(superblock->files[i].name, name) == 0) {
            return &superblock->files[i];
        }
    }
    return NULL;
}

int fs_read_file(const char* name, void* buf, uint32_t size, uint32_t offset) {
    file_entry_t* entry = fs_find_file(name);
    if (!entry) {
        return -1;
    }
    
    if (offset >= entry->size) {
        return 0;
    }
    
    uint32_t to_read = size;
    if (offset + to_read > entry->size) {
        to_read = entry->size - offset;
    }
    
    /* Calculate block and offset within block */
    uint32_t block_offset = offset % BLOCK_SIZE;
    uint32_t start_block = entry->start_block + (offset / BLOCK_SIZE);
    uint32_t blocks_to_read = ((block_offset + to_read) + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    /* Read from disk */
    uint8_t* data = (uint8_t*)fs_base + (start_block * BLOCK_SIZE) + block_offset;
    memcpy(buf, data, to_read);
    
    return to_read;
}

int fs_write_file(const char* name, const void* buf, uint32_t size, uint32_t offset) {
    file_entry_t* entry = fs_find_file(name);
    if (!entry) {
        /* Create file if it doesn't exist */
        if (fs_create_file(name, offset + size) != 0) {
            return -1;
        }
        entry = fs_find_file(name);
        if (!entry) {
            return -1;
        }
    }
    
    /* Expand file if necessary */
    if (offset + size > entry->size) {
        uint32_t new_size = offset + size;
        uint32_t new_blocks = (new_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if (new_blocks > entry->blocks) {
            /* Would need to allocate more blocks - simplified for now */
            entry->size = new_size;
        }
    }
    
    /* Calculate block and offset within block */
    uint32_t block_offset = offset % BLOCK_SIZE;
    uint32_t start_block = entry->start_block + (offset / BLOCK_SIZE);
    
    /* Write to disk */
    uint8_t* data = (uint8_t*)fs_base + (start_block * BLOCK_SIZE) + block_offset;
    memcpy(data, buf, size);
    
    return size;
}

int fs_list_files(char* buf, size_t buf_size) {
    if (!superblock) {
        return -1;
    }
    
    int pos = 0;
    for (int i = 0; i < superblock->num_files && pos < (int)buf_size - 1; i++) {
        int len = strlen(superblock->files[i].name);
        if (pos + len + 2 < (int)buf_size) {
            memcpy(buf + pos, superblock->files[i].name, len);
            pos += len;
            buf[pos++] = '\n';
        }
    }
    buf[pos] = '\0';
    return pos;
}


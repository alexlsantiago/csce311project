#include "fs.h"
#include "memory.h"
#include "string.h"
#include "types.h"
#include "kernel.h"
// #include "drivers/virtio.h"  // Not needed; disk is treated as memory-mapped

/* Superblock lives at the start of a memory-mapped region */
static fs_superblock_t* superblock = NULL;
static void* fs_base = NULL;

int fs_init(void) {
    /* Memory-mapped disk area (see README / linker layout) */
    fs_base = (void*)0xA0000000;
    superblock = (fs_superblock_t*)fs_base;

    /* Check if filesystem exists (magic number) */
    if (superblock->magic != FS_MAGIC) {
        /* Initialize new filesystem */
        memset(superblock, 0, sizeof(fs_superblock_t));
        superblock->magic = FS_MAGIC;
        superblock->num_files = 0;
        memset(superblock->block_bitmap, 0, sizeof(superblock->block_bitmap));

        /* Mark the blocks used by the superblock itself */
        int superblock_blocks =
            (int)((sizeof(fs_superblock_t) + BLOCK_SIZE - 1) / BLOCK_SIZE);

        for (int i = 0; i < superblock_blocks; i++) {
            int byte_index = i / 8;
            int bit_index  = i % 8;
            superblock->block_bitmap[byte_index] |= (1 << bit_index);
        }
    }

    return 0;
}

/* Find a run of num_blocks free blocks; mark them used and return start index */
static int find_free_blocks(int num_blocks) {
    if (!superblock) return -1;

    for (int i = 0; i <= FS_BLOCKS - num_blocks; i++) {
        int found = 1;
        for (int j = 0; j < num_blocks; j++) {
            int block = i + j;
            int byte_index = block / 8;
            int bit_index  = block % 8;
            if (superblock->block_bitmap[byte_index] & (1 << bit_index)) {
                found = 0;
                break;
            }
        }

        if (found) {
            /* Mark blocks as used */
            for (int j = 0; j < num_blocks; j++) {
                int block = i + j;
                int byte_index = block / 8;
                int bit_index  = block % 8;
                superblock->block_bitmap[byte_index] |= (1 << bit_index);
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

    /* Don't allow duplicate names */
    if (fs_find_file(name)) {
        return -1;
    }

    int num_blocks = (int)((size + BLOCK_SIZE - 1) / BLOCK_SIZE);
    int start_block = find_free_blocks(num_blocks);
    if (start_block < 0) {
        return -1;
    }

    file_entry_t* entry = &superblock->files[superblock->num_files++];
    strncpy(entry->name, name, MAX_FILENAME - 1);
    entry->name[MAX_FILENAME - 1] = '\0';  /* ensure null-termination */

    entry->size        = size;
    entry->start_block = (uint32_t)start_block;
    entry->blocks      = (uint32_t)num_blocks;
    entry->type        = 0; /* e.g., regular file */

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

    /* Compute location in the memory-mapped region */
    uint32_t block_offset = offset % BLOCK_SIZE;
    uint32_t start_block  = entry->start_block + (offset / BLOCK_SIZE);

    uint8_t* data = (uint8_t*)fs_base + (start_block * BLOCK_SIZE) + block_offset;
    memcpy(buf, data, to_read);

    return (int)to_read;
}

int fs_write_file(const char* name, const void* buf, uint32_t size, uint32_t offset) {
    file_entry_t* entry = fs_find_file(name);
    if (!entry) {
        /* Create file if it doesn't exist yet */
        if (fs_create_file(name, offset + size) != 0) {
            return -1;
        }
        entry = fs_find_file(name);
        if (!entry) {
            return -1;
        }
    }

    /* Expand file size metadata if needed
       (NOTE: this doesn't allocate extra blocks; simplification) */
    if (offset + size > entry->size) {
        uint32_t new_size   = offset + size;
        uint32_t new_blocks = (new_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if (new_blocks > entry->blocks) {
            /* Would need more blocks; simplified: just bump size and assume room */
            entry->size = new_size;
        } else {
            entry->size = new_size;
        }
    }

    /* Compute location to write to */
    uint32_t block_offset = offset % BLOCK_SIZE;
    uint32_t start_block  = entry->start_block + (offset / BLOCK_SIZE);

    uint8_t* data = (uint8_t*)fs_base + (start_block * BLOCK_SIZE) + block_offset;
    memcpy(data, buf, size);

    return (int)size;
}

int fs_list_files(char* buf, size_t buf_size) {
    if (!superblock) {
        return -1;
    }

    int pos = 0;

    for (int i = 0; i < superblock->num_files && pos < (int)buf_size - 1; i++) {
        const char* name = superblock->files[i].name;
        int len = (int)strlen(name);

        if (pos + len + 2 >= (int)buf_size) {
            break;
        }

        memcpy(buf + pos, name, len);
        pos += len;
        buf[pos++] = '\n';
    }

    buf[pos] = '\0';
    return pos;
}

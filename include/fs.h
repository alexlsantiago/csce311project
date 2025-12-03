#ifndef FS_H
#define FS_H

#include "types.h"

#define FS_MAGIC 0x4F534653  /* "OSFS" */
#define MAX_FILENAME 256
#define MAX_FILES 64
#define FS_BLOCKS 2048

typedef struct {
    char name[MAX_FILENAME];
    uint32_t size;
    uint32_t start_block;
    uint32_t blocks;
    uint8_t type;  /* 0 = file, 1 = dir */
} file_entry_t;

typedef struct {
    uint32_t magic;
    uint32_t num_files;
    file_entry_t files[MAX_FILES];
    uint8_t block_bitmap[FS_BLOCKS / 8];
} fs_superblock_t;

int fs_init(void);
int fs_create_file(const char* name, uint32_t size);
int fs_read_file(const char* name, void* buf, uint32_t size, uint32_t offset);
int fs_write_file(const char* name, const void* buf, uint32_t size, uint32_t offset);
int fs_list_files(char* buf, size_t buf_size);
file_entry_t* fs_find_file(const char* name);

#endif


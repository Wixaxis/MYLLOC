#ifndef MYLLOC_H
#define MYLLOC_H

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define BUGGED

#ifdef BUGGED
#include "self_debugger.h"
#endif //BUGGED

#define FENCE_SIZE 100
#define KB_SIZE 1024
#define PAGE_SIZE (4 * KB_SIZE)
#define BRK_ERR ((void *)-1)
#define MEM_SIZE2CHUNK_SIZE(SIZE) (SIZE + sizeof(_chunk))
#define CHUNK_SIZE2MEM_SIZE(SIZE) (SIZE - sizeof(_chunk))
#define MEM2CHUNK(PTR) (((_chunk *)PTR) + 1)
#define CHUNK2MEM(PTR) ((void *)(((_chunk *)PTR) - 1))
#define SIZE2PAGES(SIZE) (SIZE / PAGE_SIZE + !!(SIZE % PAGE_SIZE))

typedef struct _fence
{
    char fens[FENCE_SIZE]; 
} fence;

typedef struct _chunk_t
{
    fence fence_left;
    _chunk *next_chunk;
    _chunk *prev_chunk;
    uint64_t mem_size;
    bool is_free;
    fence fence_right;

} _chunk;

typedef struct _heap_t
{
    uint64_t heap_size;
    _chunk *first_chunk;
    _chunk *last_chunk;
    uint64_t fence_sum;
} _heap;

_heap heap;

void *init_heap();

bool first_used;

void feed(char *msg);

void *mylloc(size_t size);

void set_fences(_chunk *chunk);

#endif
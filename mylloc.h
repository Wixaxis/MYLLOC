#ifndef MYLLOC_H
#define MYLLOC_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include "custom_unistd.h"

#define SBRK_FAIL (void *)-1

#define DEBUG
#ifdef DEBUG
#include "self_debugger.h"
#else
void feedback(char *msg);
void display_errs();
void empty_feed(void);
void feed_init(void);
#endif

#define FENCE_SIZE 26
#define KB_SIZE 1024
#define PAGE_SIZE (4 * KB_SIZE)
#define CHUNK_SIZE (sizeof(_chunk))
#define BRK_ERR ((void *)-1)
#define MEM_SIZE2CHUNK_SIZE(SIZE) (SIZE + sizeof(_chunk))
#define CHUNK_SIZE2MEM_SIZE(SIZE) (SIZE - sizeof(_chunk))
#define MEM2CHUNK(PTR) (((_chunk *)PTR) - 1)
#define CHUNK2MEM(PTR) ((void *)(((_chunk *)PTR) + 1))
#define SIZE2PAGES(SIZE) (SIZE / PAGE_SIZE + !!(SIZE % PAGE_SIZE))
#define MAX(FIRST, SECOND) ((FIRST) > (SECOND) ? FIRST : SECOND)

enum lock_t
{
    lock,
    unlock
};

typedef struct __attribute__((packed)) _fence
{
    char fens[FENCE_SIZE];
} fence;

typedef struct __attribute__((packed)) _chunk_t
{
    fence fence_left;
    struct _chunk_t *next_chunk;
    struct _chunk_t *prev_chunk;
    int fileline;
    const char *filename;
    uint64_t mem_size;
    bool is_free;
    fence fence_right;

} _chunk;

typedef struct __attribute__((packed)) _heap_t
{
    _chunk *first_chunk;
    long long fence_sum;
    uint64_t pages_allocated;
    uint64_t chunk_count;
} _heap;

_heap heap;

pthread_mutex_t myllock_mutex;

int heap_setup(void);

void MYLOCK(enum lock_t state);

void *heap_malloc(size_t count);
void *heap_calloc(size_t number, size_t size);
void *heap_realloc(void *memblock, size_t size);

void heap_free(void *memblock);
void heap_free_all(void);

void *heap_malloc_debug(size_t count, int fileline, const char *filename);
void *heap_calloc_debug(size_t number, size_t size, int fileline, const char *filename);
void *heap_realloc_debug(void *memblock, size_t size, int fileline, const char *filename);

void *heap_malloc_aligned(size_t count);
void *heap_calloc_aligned(size_t number, size_t size);
void *heap_realloc_aligned(void *memblock, size_t size);

void *heap_malloc_aligned_debug(size_t count, int fileline, const char *filename);
void *heap_calloc_aligned_debug(size_t number, size_t size, int fileline, const char *filename);
void *heap_realloc_aligned_debug(void *memblock, size_t size, int fileline, const char *filename);

size_t heap_get_used_space(void);
size_t heap_get_largest_used_block_size(void);
uint64_t heap_get_used_blocks_count(void);
size_t heap_get_free_space(void);
size_t heap_get_largest_free_area(void);
uint64_t heap_get_free_gaps_count(void);

enum pointer_type_t
{
    pointer_null,
    pointer_out_of_heap,
    pointer_control_block,
    pointer_inside_data_block,
    pointer_unallocated,
    pointer_valid
};

enum pointer_type_t get_pointer_type(const void *const pointer);

void *heap_get_data_block_start(const void *pointer);

size_t heap_get_block_size(const void *const memblock);

int heap_validate(void);

void heap_dump_debug_information(void);

bool resize_heap_pages(int pages);

void set_chunk(_chunk *chunk, _chunk *prev_chunk, _chunk *next_chunk, size_t memsize);

void set_fences(_chunk *chunk);

bool split_chunk(_chunk *chunk_to_split, size_t memsize);

_chunk *find_fitting_chunk(size_t to_allocate);
_chunk *heap_get_last_chunk(_chunk *first_chunk);
_chunk *heap_get_last_chunk_safe(_chunk *first_chunk);

unsigned long long distance_from_start(void *ptr);

void coalesce_if_possible(_chunk *chunk);

int has_page_horison_innit(_chunk *chunk);

_chunk *find_fitting_chunk_aligned(size_t to_allocate);

bool is_pointer_aligned(void *ptr);

void display_fences();
#endif
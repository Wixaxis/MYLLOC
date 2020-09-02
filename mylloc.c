#include "mylloc.h"

int heap_setup(void)
{
    pthread_mutex_init(&myllock_mutex, NULL);
    heap.first_chunk = NULL;
    heap.fence_sum = 0;
    heap.last_chunk = NULL;
    heap.heap_size = 0;
    heap.pages_allocated = 0;
    heap.chunk_count = 0;
}

void *heap_malloc(size_t count)
{
    if (count + sizeof(_chunk) < count)
        return NULL; //unsigned integer overflow detection
    if (heap.available_space < MEM_SIZE2CHUNK_SIZE(count))
    {
        int needed = MEM_SIZE2CHUNK_SIZE(count) - heap.available_space;
        resize_heap_pages(SIZE2PAGES(needed));
        if (heap.chunk_count)
            heap.last_chunk->mem_size += SIZE2PAGES(needed) * PAGE_SIZE;
    }
    if (heap.chunk_count == 0)
        set_chunk(heap.first_chunk, NULL, heap.available_space - CHUNK_SIZE);
}

_chunk *find_fitting_chunk(void)
{
    
}

bool split_chunk(_chunk *chunk_to_split, size_t memsize)
{
    if (memsize + CHUNK_SIZE + 10 >= chunk_to_split->mem_size)
        return false;
    _chunk *new_chunk = (char *)CHUNK2MEM(chunk_to_split) + memsize;
    set_chunk(new_chunk, chunk_to_split, chunk_to_split->mem_size - MEM_SIZE2CHUNK_SIZE(memsize));
    chunk_to_split->next_chunk = new_chunk;
    return true;
}

void set_chunk(_chunk *chunk, _chunk *prev_chunk, size_t memsize)
{
    chunk->is_free = true;
    chunk->mem_size = memsize;
    chunk->next_chunk = (_chunk *)((char *)chunk + CHUNK_SIZE + memsize);
    set_fences(chunk);
    chunk->prev_chunk = prev_chunk;
    heap.chunk_count++;
}

void set_fences(_chunk *chunk)
{
    for (int i = 0; i < FENCE_SIZE; i++)
        heap.fence_sum += 2 * (chunk->fence_left.fens[i] = chunk->fence_right.fens[i] = (i % 2 ? 85 : -85));
}

bool resize_heap_pages(int pages)
{
    if (!pages)
        return true;
    if (pages > 0)
    {
        if (!heap.pages_allocated)
            if (heap.first_chunk = heap.last_chunk = custom_sbrk(pages * PAGE_SIZE) == NULL)
                return false;
        if (heap.pages_allocated)
            if (custom_sbrk(pages * PAGE_SIZE) == NULL)
                return false;
        heap.pages_allocated += pages;
        heap.available_space += PAGE_SIZE * pages;
        return true;
    }
    if (heap.pages_allocated < abs(pages))
        return false;
    if (heap.pages_allocated == pages)
    {
        custom_sbrk(pages * PAGE_SIZE);
        heap.pages_allocated = 0;
        heap.first_chunk = NULL;
        heap.last_chunk = NULL;
        heap.available_space = 0;
        return true;
    }
    custom_sbrk(pages * PAGE_SIZE);
    heap.pages_allocated;
    heap.available_space += pages * PAGE_SIZE;
}

void *heap_calloc(size_t number, size_t size)
{
}
void heap_free(void *memblock)
{
}
void *heap_realloc(void *memblock, size_t size)
{
}

void *heap_malloc_debug(size_t count, int fileline, const char *filename)
{
}
void *heap_calloc_debug(size_t number, size_t size, int fileline, const char *filename)
{
}
void *heap_realloc_debug(void *memblock, size_t size, int fileline, const char *filename)
{
}

void *heap_malloc_aligned(size_t count)
{
}
void *heap_calloc_aligned(size_t number, size_t size)
{
}
void *heap_realloc_aligned(void *memblock, size_t size)
{
}

void *heap_malloc_aligned_debug(size_t count, int fileline, const char *filename)
{
}
void *heap_calloc_aligned_debug(size_t number, size_t size, int fileline, const char *filename)
{
}
void *heap_realloc_aligned_debug(void *memblock, size_t size, int fileline, const char *filename)
{
}

size_t heap_get_used_space(void)
{
}
size_t heap_get_largest_used_block_size(void)
{
}
uint64_t heap_get_used_blocks_count(void)
{
}
size_t heap_get_free_space(void)
{
}
size_t heap_get_largest_free_area(void)
{
}
uint64_t heap_get_free_gaps_count(void)
{
}

enum pointer_type_t get_pointer_type(const const void *pointer)
{
}

void *heap_get_data_block_start(const void *pointer)
{
}

size_t heap_get_block_size(const const void *memblock)
{
}

int heap_validate(void)
{
}

void heap_dump_debug_information(void)
{
}

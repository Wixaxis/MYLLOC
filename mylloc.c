#include "mylloc.h"

int heap_setup(void)
{
    pthread_mutex_init(&myllock_mutex, NULL);
    heap.first_chunk = NULL;
    heap.fence_sum = 0;
    heap.last_chunk = NULL;
    heap.heap_size = 0;
    heap.pages_allocated = 0;
}

void *heap_malloc(size_t count)
{
    if (count + sizeof(_chunk) < count)
        return NULL; //unsigned integer overflow detection
    if (heap.available_space < MEM_SIZE2CHUNK_SIZE(count))
    {
        int needed = MEM_SIZE2CHUNK_SIZE(count) - heap.available_space;
        resize_heap_pages(SIZE2PAGES(needed));
    }
    
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

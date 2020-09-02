#include "mylloc.h"

int heap_setup(void)
{
    heap.first_chunk = custom_sbrk(PAGE_SIZE);
    if (heap.first_chunk == NULL)
        return -1;
    pthread_mutex_init(&myllock_mutex, NULL);
    heap.fence_sum = 0;
    heap.last_chunk = NULL;
    heap.pages_allocated = 1;
    heap.chunk_count = 1;
    set_chunk(heap.first_chunk, NULL, PAGE_SIZE - CHUNK_SIZE);
}

void *heap_malloc(size_t count)
{
    if (count + sizeof(_chunk) < count)
        return NULL; //unsigned integer overflow detection
    if (heap_get_largest_free_area() < MEM_SIZE2CHUNK_SIZE(count))
    {
        int needed = MEM_SIZE2CHUNK_SIZE(count) - heap.available_space;
        resize_heap_pages(SIZE2PAGES(needed));
    }
    _chunk *fitting_chunk = find_fitting_chunk(count);
    if (NULL == fitting_chunk)
        return NULL;
    fitting_chunk->is_free = false;
    return CHUNK2MEM(fitting_chunk);
}

_chunk *find_fitting_chunk(size_t to_allocate)
{
    _chunk *curr_chunk = heap.first_chunk;
    for (int i = 0; i < heap.chunk_count; i++, curr_chunk = curr_chunk->next_chunk)
    {
        if (NULL == curr_chunk)
            return NULL;
        if (false == curr_chunk->is_free)
            continue;
        if (curr_chunk->mem_size == to_allocate)
            return curr_chunk;
        if (curr_chunk->mem_size > to_allocate + CHUNK_SIZE + 10)
        {
            split_chunk(curr_chunk, to_allocate);
            return curr_chunk;
        }
    }
    return NULL;
}

bool split_chunk(_chunk *chunk_to_split, size_t memsize)
{
    if (memsize + CHUNK_SIZE + 10 >= chunk_to_split->mem_size)
        return false;
    _chunk *new_chunk = (_chunk *)((char *)CHUNK2MEM(chunk_to_split) + memsize);
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
        if (custom_sbrk(pages * PAGE_SIZE) == NULL)
            return false;
        heap.pages_allocated += pages;
        heap.available_space += PAGE_SIZE * pages;
        // heap.
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
    heap.pages_allocated += pages;
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
    size_t space = 0;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
        if (curr_chunk->is_free == false)
            space += curr_chunk->mem_size;
    return space;
}
size_t heap_get_largest_used_block_size(void)
{
    size_t largest_space = 0;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
        if (curr_chunk->mem_size > largest_space && curr_chunk->is_free == false)
            largest_space = curr_chunk->mem_size;
    return largest_space;
}
uint64_t heap_get_used_blocks_count(void)
{
    uint64_t used = 0;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
        if (curr_chunk->is_free == false)
            used++;
    return used;
}
size_t heap_get_free_space(void)
{
    size_t space = 0;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
        if (curr_chunk->is_free)
            space += curr_chunk->mem_size;
    return space;
}
size_t heap_get_largest_free_area(void)
{
    size_t largest_space = 0;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
        if (curr_chunk->mem_size > largest_space && curr_chunk->is_free)
            largest_space = curr_chunk->mem_size;
    return largest_space;
}
uint64_t heap_get_free_gaps_count(void)
{
    uint64_t gaps = 0;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
        if (curr_chunk->is_free && curr_chunk->mem_size >= sizeof(double) + CHUNK_SIZE)
            gaps++;
    return gaps;
}

enum pointer_type_t get_pointer_type(const const void *pointer)
{
    if (NULL == pointer)
        return pointer_null;
    if (heap.first_chunk > pointer || heap.last_chunk + heap.last_chunk->mem_size + CHUNK_SIZE < pointer)
        return pointer_out_of_heap;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
    {
        if (pointer > curr_chunk->next_chunk != NULL ? curr_chunk->next_chunk : (char *)curr_chunk + CHUNK_SIZE + curr_chunk->mem_size)
            continue;
        if (pointer >= (void *)&curr_chunk->fence_left && pointer < curr_chunk + 1)
            return pointer_control_block;
        if (pointer > curr_chunk + 1 && pointer < (char *)curr_chunk + CHUNK_SIZE + curr_chunk->mem_size)
            return curr_chunk->is_free ? pointer_unallocated : pointer_inside_data_block;
        if (pointer == curr_chunk + 1)
            return pointer_valid;
    }
    return pointer_out_of_heap;
}

void *heap_get_data_block_start(const void *pointer)
{
    if (pointer_valid == get_pointer_type(pointer))
        return pointer;
    if (pointer_inside_data_block == get_pointer_type(pointer))
    {
        for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
            if (pointer > curr_chunk + 1 && pointer < (char *)curr_chunk + CHUNK_SIZE + curr_chunk->mem_size)
                return (void *)curr_chunk + 1;
    }
    return NULL;
}

size_t heap_get_block_size(const const void *memblock)
{
    if (pointer_valid != get_pointer_type(memblock))
        return NULL;
    return (size_t)((_chunk *)memblock - 1)->mem_size;
}

int heap_validate(void)
{
    long current_sum = 0;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
        for (int i = 0; i < FENCE_SIZE; i++)
            current_sum += curr_chunk->fence_left.fens[i] + curr_chunk->fence_right.fens[i];
    if (current_sum != heap.fence_sum)
        return -1;
    return 0;
}

void heap_dump_debug_information(void)
{
}

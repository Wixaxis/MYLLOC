#include "mylloc.h"

#ifndef DEBUG
void feedback(char *msg) { return; }
void display_errs() { return; }
#endif

int heap_setup(void)
{
    pthread_mutex_init(&err_mamutex, NULL);
    feedback("Heap setup starting");
    heap.first_chunk = custom_sbrk(PAGE_SIZE);
    if (heap.first_chunk == NULL)
        return feedback("sbrk returned NULL, heap setup failed"), -1;
    pthread_mutex_init(&myllock_mutex, NULL);
    heap.fence_sum = 0;
    heap.pages_allocated = 1;
    heap.chunk_count = 1;
    set_chunk(heap.first_chunk, NULL, NULL, PAGE_SIZE - CHUNK_SIZE);
    return 0;
}

void *heap_malloc(size_t count)
{
    return heap_malloc_debug(count, 0, NULL);
}

_chunk *find_fitting_chunk(size_t to_allocate)
{
    char err_hub[100] = {0};
    sprintf(err_hub, "looking for a chunk of size %lu", to_allocate);
    feedback(err_hub);
    _chunk *curr_chunk = heap.first_chunk;
    for (int i = 0; i < heap.chunk_count; i++, curr_chunk = curr_chunk->next_chunk)
    {
        if (NULL == curr_chunk)
            return NULL;
        if (false == curr_chunk->is_free)
            continue;
        if (curr_chunk->mem_size <= to_allocate + CHUNK_SIZE + sizeof(double) && curr_chunk->mem_size >= to_allocate)
            return feedback("found prefect chunk"), curr_chunk;
        if (curr_chunk->mem_size > to_allocate + CHUNK_SIZE + sizeof(double))
        {
            split_chunk(curr_chunk, to_allocate);
            return feedback("found bigger chunk, had to split it"), curr_chunk;
        }
    }
    return feedback("Couldn't find fitting chunk"), NULL;
}

bool split_chunk(_chunk *chunk_to_split, size_t memsize)
{
    char error_hub[100];
    sprintf(error_hub, "Splitting chunk size %lu of size %lu", chunk_to_split->mem_size, memsize);
    feedback(error_hub);
    if (memsize + CHUNK_SIZE + 10 >= chunk_to_split->mem_size)
        return feedback("requested mem size too big for chunk to be splitted"), false;
    _chunk *new_chunk = (_chunk *)((char *)CHUNK2MEM(chunk_to_split) + memsize);
    set_chunk(new_chunk, chunk_to_split, chunk_to_split->next_chunk, chunk_to_split->mem_size - MEM_SIZE2CHUNK_SIZE(memsize));
    chunk_to_split->next_chunk = new_chunk;
    chunk_to_split->mem_size = memsize;
    sprintf(error_hub, "chunk splitted to %lu and %lu", chunk_to_split->mem_size, chunk_to_split->next_chunk->mem_size);
    feedback(error_hub);
    return true;
}

void set_chunk(_chunk *chunk, _chunk *prev_chunk, _chunk *next_chunk, size_t memsize)
{
    char err_hub[50] = {0};
    sprintf(err_hub, "setting chunk of size %lu", memsize);
    chunk->is_free = true;
    chunk->mem_size = memsize;
    chunk->next_chunk = next_chunk;
    set_fences(chunk);
    chunk->prev_chunk = prev_chunk;
    heap.chunk_count++;
}

void set_fences(_chunk *chunk)
{
    for (int i = 0; i < FENCE_SIZE; i++)
        heap.fence_sum += 2 * (chunk->fence_left.fens[i] = chunk->fence_right.fens[i] = (i % 2 ? 85 : -85));
}

_chunk *heap_get_last_chunk(_chunk *first_chunk)
{
    if (first_chunk->next_chunk == NULL)
        return first_chunk;
    return heap_get_last_chunk(first_chunk->next_chunk);
}

bool resize_heap_pages(int pages)
{
    if (!pages)
        return true;
    if (pages > 0)
    {
        if (custom_sbrk(pages * PAGE_SIZE) == NULL)
            return feedback("sbrk returned NULL at page resize"), false;
        heap.pages_allocated += pages;
        _chunk *last_chunk = heap_get_last_chunk(heap.first_chunk);
        if (last_chunk->is_free == true)
            last_chunk->mem_size += pages * PAGE_SIZE;
        else
        {
            _chunk *new_chunk = (_chunk *)(((char *)(last_chunk + 1)) + last_chunk->mem_size);
            set_fences(new_chunk);
            new_chunk->is_free = true;
            new_chunk->fileline = 0;
            new_chunk->filename = NULL;
            new_chunk->mem_size = PAGE_SIZE - CHUNK_SIZE;
            new_chunk->prev_chunk = last_chunk;
            new_chunk->next_chunk = NULL;
            last_chunk->next_chunk = new_chunk;
        }
        return true;
    }
    if (heap.pages_allocated < abs(pages))
        return false;
    if (heap.pages_allocated == pages)
    {
        custom_sbrk(pages * PAGE_SIZE);
        heap.pages_allocated = 0;
        heap.first_chunk = NULL;
        return true;
    }
    custom_sbrk(pages * PAGE_SIZE);
    heap.pages_allocated += pages;
    _chunk *last_chunk = heap_get_last_chunk(heap.first_chunk);
    last_chunk->mem_size += pages * PAGE_SIZE;
    return true;
}

void *heap_calloc(size_t number, size_t size)
{
    return heap_calloc_debug(number, size, 0, NULL);
}
void heap_free(void *memblock)
{
    if (pointer_valid != get_pointer_type(memblock))
        return;
    _chunk *chunk = memblock;
    chunk--;
    chunk->is_free = true;
    _chunk *last_chunk = heap_get_last_chunk(heap.first_chunk);
    if (last_chunk->is_free && last_chunk->mem_size > PAGE_SIZE)
        resize_heap_pages(-1);
    coalesce_if_possible(last_chunk);
}

void coalesce_if_possible(_chunk *chunk)
{
    if (chunk == NULL)
        return;
    if (chunk->is_free == false)
        return coalesce_if_possible(chunk->prev_chunk);
    if (chunk->next_chunk != NULL && chunk->next_chunk->is_free)
    {
        _chunk *to_destroy = chunk->next_chunk;
        chunk->next_chunk = to_destroy->next_chunk;
        chunk->mem_size += to_destroy->mem_size + CHUNK_SIZE;
        for (int i = 0; i < FENCE_SIZE; i++)
            heap.fence_sum -= to_destroy->fence_left.fens[i] + to_destroy->fence_right.fens[i];
        heap.chunk_count--;
    }
    coalesce_if_possible(chunk->prev_chunk);
}

void *heap_realloc(void *memblock, size_t size)
{
    return heap_realloc_debug(memblock, size, 0, NULL);
}

void *heap_malloc_debug(size_t count, int fileline, const char *filename)
{
    char err_hub[200] = {0};
    sprintf(err_hub, "trying to allocate %lu bytes in line %d", count, fileline);
    feedback(err_hub);
    if (count + sizeof(_chunk) < count)
        return feedback("Got too big of a number to allocate"), NULL; //unsigned integer overflow detection
    if (heap_get_largest_free_area() < MEM_SIZE2CHUNK_SIZE(count))
    {
        int needed = 0;
        feedback("No chunk big enough, asking for more pages");
        _chunk *last_chunk = heap_get_last_chunk(heap.first_chunk);
        if (last_chunk->is_free == true)
            needed = count - last_chunk->mem_size;
        else
            needed = count + CHUNK_SIZE;
        resize_heap_pages(SIZE2PAGES(needed));
    }
    _chunk *fitting_chunk = find_fitting_chunk(count);
    if (NULL == fitting_chunk)
        return NULL;
    fitting_chunk->is_free = false;
    fitting_chunk->filename = filename;
    fitting_chunk->fileline = fileline;
    return CHUNK2MEM(fitting_chunk);
}
void *heap_calloc_debug(size_t number, size_t size, int fileline, const char *filename)
{
    if (number + size < number)
        return NULL;
    void *ptr = heap_malloc_debug(size * number, fileline, filename);
    if (NULL == ptr)
        return NULL;
    return memset(ptr, 0, size * number);
}
void *heap_realloc_debug(void *memblock, size_t size, int fileline, const char *filename)
{
    if (pointer_valid != get_pointer_type(memblock))
        return NULL;
    _chunk *chunk = find_fitting_chunk(size);
    if (NULL == chunk)
    {
        chunk = heap_malloc(size);
        if (NULL == chunk)
            return NULL;
    }
    memcpy(chunk + 1, memblock, size);
    chunk->fileline = fileline;
    chunk->filename = filename;
    heap_free(memblock);
    return chunk + 1;
}

void *heap_malloc_aligned(size_t count)
{
    return heap_malloc_aligned_debug(count, 0, NULL);
}
void *heap_calloc_aligned(size_t number, size_t size)
{
    return heap_calloc_aligned_debug(number, size, 0, NULL);
}
void *heap_realloc_aligned(void *memblock, size_t size)
{
    return heap_realloc_aligned_debug(memblock, size, 0, NULL);
}

void *heap_malloc_aligned_debug(size_t count, int fileline, const char *filename)
{
    if (count + CHUNK_SIZE < count)
        return NULL;
    _chunk *new_chunk = custom_sbrk(SIZE2PAGES(count + CHUNK_SIZE));
    if (NULL == new_chunk)
        return NULL;
    heap.pages_allocated++;
    set_fences(new_chunk);
    new_chunk->is_free = true;
    new_chunk->mem_size = (SIZE2PAGES(count + CHUNK_SIZE) * PAGE_SIZE) - CHUNK_SIZE;
    new_chunk->prev_chunk = heap_get_last_chunk(heap.first_chunk);
    new_chunk->prev_chunk->next_chunk = new_chunk;
    new_chunk->next_chunk = NULL;
    new_chunk->fileline = fileline;
    new_chunk->filename = filename;
    if (new_chunk->mem_size >= count && new_chunk->mem_size <= count + CHUNK_SIZE + sizeof(double))
        return new_chunk->is_free = false, (void *)new_chunk;
    split_chunk(new_chunk, count);
    return new_chunk;
}
void *heap_calloc_aligned_debug(size_t number, size_t size, int fileline, const char *filename)
{
    void *memory = heap_malloc_aligned_debug(number * size, fileline, filename);
    if (NULL == memory)
        return NULL;
    return memset(memory, 0, number);
}
void *heap_realloc_aligned_debug(void *memblock, size_t size, int fileline, const char *filename)
{
    if (pointer_valid != get_pointer_type(memblock))
        return NULL;
    int old_size = ((_chunk *)memblock - 1)->mem_size;
    void *memory = heap_malloc_aligned_debug(size, fileline, filename);
    if (NULL == memory)
        return NULL;
    return memcpy(memory, memblock, size > old_size ? old_size : size);
}

bool block_page_horizon_check(_chunk *chunk)
{
    char *block_begin = (char *)(chunk + 1);
    char *block_end = block_begin + chunk->mem_size;
    return (distance_from_start(block_begin) / PAGE_SIZE) != (distance_from_start(block_end - 1) / PAGE_SIZE) ? true : false;
}

unsigned long long distance_from_start(void *ptr)
{
    return (unsigned long long)((char *)ptr - (char *)heap.first_chunk);
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

enum pointer_type_t get_pointer_type(const void *const pointer)
{
    if (NULL == pointer)
        return pointer_null;
    _chunk *last_chunk = heap_get_last_chunk(heap.first_chunk);
    if ((void *)heap.first_chunk > pointer || (void *)((char *)last_chunk + last_chunk->mem_size + CHUNK_SIZE) < pointer)
        return pointer_out_of_heap;
    for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
    {
        if (pointer > (curr_chunk->next_chunk != NULL ? (void *)(curr_chunk->next_chunk) : (void *)((char *)curr_chunk + CHUNK_SIZE + curr_chunk->mem_size)))
            continue;
        if (pointer >= (void *)&curr_chunk->fence_left && pointer < (void *)(curr_chunk + 1))
            return pointer_control_block;
        if (pointer > (void *)(curr_chunk + 1) && pointer < (void *)((char *)curr_chunk + CHUNK_SIZE + curr_chunk->mem_size))
            return curr_chunk->is_free ? pointer_unallocated : pointer_inside_data_block;
        if (pointer == (void *)(curr_chunk + 1))
            return pointer_valid;
    }
    return pointer_out_of_heap;
}

void *heap_get_data_block_start(const void *pointer)
{
    if (pointer_valid == get_pointer_type(pointer))
        return (void *)pointer;
    if (pointer_inside_data_block == get_pointer_type(pointer))
    {
        for (_chunk *curr_chunk = heap.first_chunk; curr_chunk != NULL; curr_chunk = curr_chunk->next_chunk)
            if (pointer > (void *)(curr_chunk + 1) && pointer < (void *)((char *)curr_chunk + CHUNK_SIZE + curr_chunk->mem_size))
                return (void *)curr_chunk + 1;
    }
    return NULL;
}

size_t heap_get_block_size(const void *const memblock)
{
    if (pointer_valid != get_pointer_type(memblock))
        return 0;
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
    printf("==============\nchunk size: %ld\n", CHUNK_SIZE);
    for (_chunk *chunk = heap.first_chunk; chunk != NULL; chunk = chunk->next_chunk)
    {
        printf("chunk address: %p\ndistance from start: %llu\nmemory block address: %p\nis free: %s\nmemory block length: %lu\nfilename: %s\nline: ", chunk, distance_from_start((void *)chunk), chunk + 1, chunk->is_free == true ? "YES" : "NO", chunk->mem_size, chunk->filename == NULL ? "UNKNOWN" : chunk->filename);
        chunk->fileline == 0 ? printf("UNKNOWN\n--------------\n") : printf("%d\n--------------\n", chunk->fileline);
    }
    printf("\nheap size: %lu\nallocated space: %lu + ( %lu )\nfree space: %lu\nbiggest free block size: %lu\n==============\n", heap.pages_allocated * PAGE_SIZE, heap_get_used_space(), heap.chunk_count * CHUNK_SIZE, heap_get_free_space(), heap_get_largest_free_area());
}

#include "unit_tests.h"

int main(void)
{
    test_heap_setup();
    printf("\n\n");
    test_malloc_and_free();
    printf("\n\n");
    test_calloc();
    printf("\n\n");
    test_realloc();
    printf("\n\n");
    test_malloc_aligned();
    printf("\n\n");
    test_calloc_aligned();
    printf("\n\n");
    return 0;
}
void destroy_heap(void)
{
    if (heap.first_chunk == NULL)
        return;
    heap_free_all();
    memset(heap.first_chunk, 0, heap.pages_allocated * PAGE_SIZE);
    custom_sbrk(-1 * heap.pages_allocated);
    heap.chunk_count = 0;
    heap.fence_sum = 0;
    heap.first_chunk = NULL;
    heap.pages_allocated = 0;
}
void heap_show_short(void)
{
    printf("HEAP:\n");
    if (heap.first_chunk == NULL)
        return (void)printf("[heap empty]\n");
    _chunk *chunk = heap.first_chunk;
    for (int i = 0; chunk != NULL; i++, chunk = chunk->next_chunk)
        printf("Chunk[%d]:[%s][%lu]\n", i, chunk->is_free ? "FREE" : "BUSY", chunk->mem_size);
    printf("------------\n");
}

bool test_heap_setup(void)
{
    printf("HEAP_SETUP_TEST Starting...");
    int status = heap_setup();
    if (status)
        return printf("heap setup failed on empty heap"), false;
    printf(" Heap set...");
    status = heap_setup();
    if (!status)
        return printf("heap_setup set heap on already existing heap"), false;
    printf(" second heap_setup failed(good)...");
    destroy_heap();
    status = heap_setup();
    if (status)
        return printf("heap_setup failed on empty heap"), false;
    destroy_heap();
    printf("HEAP_SETUP_TEST Success. Heap clear\n");
    return true;
}

bool test_malloc_and_free(void)
{
    int test_size = 15;
    printf("MALLOC_AND_FREE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    int *first = heap_malloc(sizeof(int));
    if (first == NULL)
        return printf("malloc failed to allocate sizeof(int) bytes"), false;
    heap_free(first);
    printf(" One pointer allocation and free done... Allocating %d ptrs of different sizes...", test_size);
    int *ptrs[test_size];
    for (int i = 0; i < test_size; i++)
        ptrs[i] = heap_malloc(sizeof(int) * (i + 1));
    int check = 0;
    for (int i = 0; i < test_size; i++)
        if (ptrs[i] == NULL)
            check = (printf("ptrs[%d] failed, ", i), 1);
    if (check)
    {
        printf("ALLOCATION FAIL, ABORTED\n");
        destroy_heap();
        return false;
    }
    printf(" allocated all, showing heap... ");
    heap_show_short();
    printf("freeing all... ");
    for (int i = 0; i < test_size; i++)
        heap_free(ptrs[i]);
    heap_show_short();
    for (int i = 0; i < test_size; i++)
        ptrs[i] = heap_malloc(sizeof(int) * (i + 1));
    heap_show_short();
    for (int i = 0; i < test_size; i += 2)
        heap_free(ptrs[i]);
    heap_show_short();
    printf("Allocating 20 bytes\n");
    int *last_test = heap_malloc(20);
    heap_show_short();
    heap_free_all();
    destroy_heap();
    printf("MALLOC_AND_FREE TEST SUCCESS");
    return true;
}

bool test_calloc(void)
{
    int test_size = 15;
    printf("CALLOC TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    int *ptrs[test_size];
    for (int i = 0; i < test_size; i++)
        ptrs[i] = heap_calloc(i + 1, sizeof(int));
    int check = 0;
    for (int i = 0; i < test_size; i++)
        if (ptrs[i] == NULL)
            check = (printf("ptrs[%d] failed, ", i), 1);
    if (check)
    {
        printf("allocation failed!");
        heap_free_all();
        destroy_heap();
        return false;
    }
    for (int i = 0; i < test_size; i++)
        for (int j = 0; j < i + 1; j++)
            if (ptrs[i][j])
            {
                check = 1;
                printf("ptrs[%d] not empty!\n", i);
                continue;
            }
    if (check)
    {
        printf("allocation not cleared!");
        heap_free_all();
        destroy_heap();
        return false;
    }
    heap_free_all();
    destroy_heap();
    return printf("CALLOC TEST SUCCESS"), true;
}

bool test_realloc(void)
{
    printf("REALLOC TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... Allocating 20 bytes...");
    void *pointer = heap_malloc(20);
    if (NULL == pointer)
        return destroy_heap(), printf("ALLOCATION FAILED!"), false;
    heap_show_short();
    printf("Reallocating 20 to 30 bytes... ");
    void *second_pointer = heap_realloc(pointer, 30);
    if (NULL == second_pointer)
        return destroy_heap(), printf("REALLOCATION FAILED!"), false;
    heap_show_short();
    printf("Allocating new 20... ");
    void *third_pointer = heap_malloc(20);
    if (NULL == third_pointer)
        return destroy_heap(), printf("ALLOCATION FAILED!"), false;
    heap_show_short();
    printf("free 20 bytes and reallocate 30->5 bytes... ");
    heap_free(third_pointer);
    third_pointer = heap_realloc(second_pointer, 5);
    if (NULL == third_pointer)
        return destroy_heap(), printf("REALLOCATION FAILED!"), false;
    heap_show_short();
    destroy_heap();
    printf("REALLOC TEST SUCCESS\n");
    return true;
}

bool test_malloc_aligned(void)
{
    printf("MALLOC ALIGNED TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... Allocating 20 bytes...");
    void *ptr1 = heap_malloc_aligned(20);
    if (NULL == ptr1)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr1) || distance_from_start(ptr1) / PAGE_SIZE)
        return destroy_heap(), printf("block not at the beginning of page! FAIL"), false;
    heap_show_short();
    printf("Allocating next 20... ");
    void *ptr2 = heap_malloc_aligned(20);
    if (NULL == ptr2)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr2) || distance_from_start(ptr2) / PAGE_SIZE)
        return destroy_heap(), printf("block not at the beginning of page! FAIL"), false;
    heap_show_short();
    printf("Free first alloc...");
    heap_free(ptr1);
    heap_show_short();
    destroy_heap();
    printf("MALLOC ALIGNED TEST SUCCESS");
    return true;
}
bool test_calloc_aligned(void)
{
    printf("CALLOC ALIGNED TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... Allocating 20 bytes...");
    char *ptr1 = heap_calloc_aligned(20, sizeof(char));
    if (NULL == ptr1)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr1) || distance_from_start(ptr1) / PAGE_SIZE)
        return destroy_heap(), printf("block not at the beginning of page! FAIL"), false;
    for (int i = 0; i < 20; i++)
        if (ptr1[i])
            return destroy_heap(), printf("block not empty! FAIL"), false;
    heap_show_short();
    printf("Allocating next 20... ");
    char *ptr2 = heap_calloc_aligned(20, sizeof(char));
    if (NULL == ptr2)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr2) || distance_from_start(ptr2) / PAGE_SIZE)
        return destroy_heap(), printf("block not at the beginning of page! FAIL"), false;
    for (int i = 0; i < 20; i++)
        if (ptr2[i])
            return destroy_heap(), printf("block not empty! FAIL"), false;
    heap_show_short();
    printf("Free first alloc...");
    heap_free(ptr1);
    heap_show_short();
    destroy_heap();
    printf("MALLOC ALIGNED TEST SUCCESS");
    return true;
}
bool test_realloc_aligned(void)
{

    return true;
}

bool test_get_used_space(void)
{

    return true;
}

bool test_get_largest_used_block_size(void)
{

    return true;
}
bool test_get_used_blocks_count(void)
{

    return true;
}
bool test_get_free_space(void)
{

    return true;
}
bool test_get_largest_free_area(void)
{

    return true;
}
bool test_get_free_gaps_count(void)
{

    return true;
}

bool test_get_pointer_type(const void *const pointer)
{

    return true;
}

bool test_get_data_block_start(const void *pointer)
{

    return true;
}

bool test_get_block_size(const void *const memblock)
{

    return true;
}

bool test_validate(void)
{

    return true;
}

bool test_resize_heap_pages(int pages)
{

    return true;
}

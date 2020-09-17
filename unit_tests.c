#include "unit_tests.h"

int heaps_dont_lie = 0;

int run_tests(void)
{
    int succ = 0, all = 0;
    succ += !!test_heap_setup();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_malloc_and_free();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_calloc();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_realloc();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_malloc_aligned();
    all++;
    printf("\n\n");
    // display_errs();
    empty_feed();

    succ += !!test_calloc_aligned();
    all++;
    printf("\n\n");
    // display_errs();
    empty_feed();

    succ += !!test_realloc_aligned();
    all++;
    printf("\n\n");
    // display_errs();
    empty_feed();

    succ += !!test_get_used_space();
    all++;
    printf("\n\n");
    // display_errs();
    empty_feed();

    succ += !!test_get_largest_used_block_size();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_get_used_blocks_count();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_get_free_space();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_get_largest_free_area();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_get_free_gaps_count();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_get_pointer_type();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_get_data_block_start();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_get_block_size();
    all++;
    printf("\n\n");
    //display_errs();
    empty_feed();

    succ += !!test_validate();
    all++;
    printf("\n\n");
    // display_errs();
    empty_feed();

    succ += !!test_multithreaded_workload();
    all++;
    printf("\n\n");
    // display_errs();
    empty_feed();

    printf("SUCCESSS IN %d out of %d\n", succ, all);
    return succ;
}
void destroy_heap(void)
{
    if (heap.first_chunk == NULL)
        return;
    // memset(heap.first_chunk, 0, heap.pages_allocated * PAGE_SIZE);
    heap_free_all();
    heap.chunk_count = 0;
    heap.fence_sum = 0;
    heap.first_chunk = NULL;
    heap.pages_allocated = 0;
}
void heap_show_short(void)
{
    if (heaps_dont_lie == 0)
        return;
    printf("HEAP:\n");
    if (heap.first_chunk == NULL)
        return (void)printf("[heap empty]\n");
    _chunk *chunk = heap.first_chunk;
    for (int i = 0; chunk != NULL; i++, chunk = chunk->next_chunk)
        printf("Chunk[%d]:[%s][%lu]\n", i, chunk->is_free ? "FREE" : "BUSY", chunk->mem_size);
    printf("------------\n");
}

void heap_show_short_alignment(void)
{
    if (heaps_dont_lie == 0)
        return;
    printf("HEAP:\n");
    if (heap.first_chunk == NULL)
        return (void)printf("[heap empty]\n");
    _chunk *chunk = heap.first_chunk;
    for (int i = 0; chunk != NULL; i++, chunk = chunk->next_chunk)
        printf("Chunk[%d]:[%s][%lu](start + %llu)\n", i, chunk->is_free ? "FREE" : "BUSY", chunk->mem_size, distance_from_start(CHUNK2MEM(chunk)));
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
    heap_show_short_alignment();
    if (NULL == ptr1)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr1) || (intptr_t)ptr1 & (intptr_t)(PAGE_SIZE - 1))
        return destroy_heap(), printf("block not at the beginning of page! FAIL %ld bytes off", (intptr_t)ptr1 & (intptr_t)(PAGE_SIZE - 1)), false;
    heap_show_short_alignment();
    printf("Allocating next 20... ");
    void *ptr2 = heap_malloc_aligned(20);
    heap_show_short_alignment();
    if (NULL == ptr2)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr2) || (intptr_t)ptr2 & (intptr_t)(PAGE_SIZE - 1))
        return destroy_heap(), printf("block not at the beginning of page! FAIL %ld bytes off", (intptr_t)ptr2 & (intptr_t)(PAGE_SIZE - 1)), false;
    heap_show_short_alignment();
    printf("Free first alloc...");
    heap_free(ptr1);
    heap_show_short_alignment();
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
    heap_show_short_alignment();
    if (NULL == ptr1)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr1) || (intptr_t)ptr1 & (intptr_t)(PAGE_SIZE - 1))
        return destroy_heap(), printf("block not at the beginning of page! FAIL %ld bytes off", (intptr_t)ptr1 & (intptr_t)(PAGE_SIZE - 1)), false;
    for (int i = 0; i < 20; i++)
        if (ptr1[i])
            return destroy_heap(), printf("block not empty! FAIL"), false;
    heap_show_short_alignment();
    printf("Allocating next 20... ");
    char *ptr2 = heap_calloc_aligned(20, sizeof(char));
    heap_show_short_alignment();
    if (NULL == ptr2)
        return destroy_heap(), printf("ALLOCATION FAILED"), false;
    if (pointer_valid != get_pointer_type(ptr2) || (intptr_t)ptr2 & (intptr_t)(PAGE_SIZE - 1))
        return destroy_heap(), printf("block not at the beginning of page! FAIL %ld bytes off", (intptr_t)ptr2 & (intptr_t)(PAGE_SIZE - 1)), false;
    for (int i = 0; i < 20; i++)
        if (ptr2[i])
            return destroy_heap(), printf("block not empty! FAIL"), false;
    heap_show_short_alignment();
    printf("Free first alloc...");
    heap_free(ptr1);
    heap_show_short_alignment();
    destroy_heap();
    printf("MALLOC ALIGNED TEST SUCCESS");
    return true;
}
bool test_realloc_aligned(void)
{
    printf("REALLOC ALIGNED TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... Allocating 20 bytes...");
    void *pointer = heap_malloc_aligned(20);
    if (NULL == pointer)
        return destroy_heap(), printf("ALLOCATION FAILED!"), false;
    heap_show_short();
    printf("Reallocating 20 to 30 bytes... ");
    void *second_pointer = heap_realloc_aligned(pointer, 30);
    if (NULL == second_pointer)
        return destroy_heap(), printf("REALLOCATION FAILED!"), false;
    heap_show_short();
    printf("Allocating new 20... ");
    void *third_pointer = heap_malloc_aligned(20);
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

bool test_get_used_space(void)
{
    printf("GET USED SPACE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    void *ptr1, *ptr2, *ptr3;
    if (heap_get_used_space() != 0)
        return false;
    ptr1 = heap_malloc(20);
    if (heap_get_used_space() != 20)
        return false;
    ptr2 = heap_calloc(40, 1);
    if (heap_get_used_space() != 20 + 40)
        return false;
    ptr3 = heap_malloc(100);
    if (heap_get_used_space() != 20 + 40 + 100)
        return false;
    heap_free(ptr2);
    if (heap_get_used_space() != 20 + 100)
        return false;
    heap_free(ptr1);
    if (heap_get_used_space() != 100)
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}

bool test_get_largest_used_block_size(void)
{
    printf("GET LARGEST USED BLOCK SIZE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    void *ptr1, *ptr2, *ptr3;
    if (heap_get_largest_used_block_size() != 0)
        return false;
    ptr1 = heap_malloc(20);
    if (heap_get_largest_used_block_size() != 20)
        return false;
    ptr2 = heap_calloc(40, 1);
    if (heap_get_largest_used_block_size() != 40)
        return false;
    ptr3 = heap_malloc(100);
    if (heap_get_largest_used_block_size() != 100)
        return false;
    heap_free(ptr3);
    if (heap_get_largest_used_block_size() != 40)
        return false;
    heap_free(ptr2);
    if (heap_get_largest_used_block_size() != 20)
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}
bool test_get_used_blocks_count(void)
{
    printf("GET USED BLOCK COUNT TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    void *ptr1, *ptr2, *ptr3;
    if (heap_get_used_blocks_count() != 0)
        return false;
    ptr1 = heap_malloc(20);
    if (heap_get_used_blocks_count() != 1)
        return false;
    ptr2 = heap_calloc(40, 1);
    if (heap_get_used_blocks_count() != 2)
        return false;
    ptr3 = heap_malloc(100);
    if (heap_get_used_blocks_count() != 3)
        return false;
    heap_free(ptr3);
    if (heap_get_used_blocks_count() != 2)
        return false;
    heap_free(ptr2);
    if (heap_get_used_blocks_count() != 1)
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}
bool test_get_free_space(void)
{
    printf("GET FREE SPACE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    void *ptr1, *ptr2, *ptr3;
    if (heap_get_free_space() != PAGE_SIZE - CHUNK_SIZE)
        return false;
    ptr1 = heap_malloc(20);
    if (heap_get_free_space() != PAGE_SIZE - CHUNK_SIZE * 2 - 20)
        return false;
    ptr2 = heap_calloc(40, 1);
    if (heap_get_free_space() != PAGE_SIZE - CHUNK_SIZE * 3 - 20 - 40)
        return false;
    ptr3 = heap_malloc(100);
    if (heap_get_free_space() != PAGE_SIZE - CHUNK_SIZE * 4 - 20 - 40 - 100)
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}
bool test_get_largest_free_area(void)
{
    printf("GET LARGEST FREE AREA TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    void *ptr1, *ptr2, *ptr3;
    if (heap_get_largest_free_area() != PAGE_SIZE - CHUNK_SIZE)
        return false;
    ptr1 = heap_malloc(20);
    if (heap_get_largest_free_area() != PAGE_SIZE - CHUNK_SIZE * 2 - 20)
        return false;
    ptr2 = heap_calloc(40, 1);
    if (heap_get_largest_free_area() != PAGE_SIZE - CHUNK_SIZE * 3 - 20 - 40)
        return false;
    ptr3 = heap_malloc(100);
    if (heap_get_largest_free_area() != PAGE_SIZE - CHUNK_SIZE * 4 - 20 - 40 - 100)
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}
bool test_get_free_gaps_count(void)
{
    printf("GET FREE SPACE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    void *ptr1, *ptr2, *ptr3;
    if (heap_get_free_gaps_count() != 1)
        return false;
    ptr1 = heap_malloc(20);
    ptr2 = heap_calloc(40, sizeof(double));
    ptr3 = heap_malloc(100);
    heap_free(ptr2);
    if (heap_get_free_gaps_count() != 2)
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}

bool test_get_pointer_type(void)
{
    printf("GET POINTER TYPE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    char *ptr = (char *)heap.first_chunk;
    if (pointer_control_block != get_pointer_type(ptr + 5))
        return false;
    if (pointer_null != get_pointer_type(NULL))
        return false;
    if (pointer_out_of_heap != get_pointer_type(ptr - 20))
        return false;
    if (pointer_out_of_heap != get_pointer_type(ptr + PAGE_SIZE * 2))
        return false;
    if (pointer_unallocated != get_pointer_type(ptr + CHUNK_SIZE + 5))
        return false;
    void *ptr2 = heap_malloc(sizeof(int));
    if (pointer_valid != get_pointer_type(ptr2))
        return false;
    if (pointer_inside_data_block != get_pointer_type((char *)ptr2 + sizeof(int) / 2))
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}

bool test_get_data_block_start(void)
{
    printf("GET DATA BLOCK START TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    int *ptr = heap_malloc(20 * sizeof(int));
    if (heap_get_data_block_start(ptr + 5) != (void *)ptr)
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}

bool test_get_block_size(void)
{
    printf("GET DATA BLOCK SIZE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    int *ptr = heap_malloc(20 * sizeof(int));
    if (heap_get_block_size(ptr) != 20 * sizeof(int))
        return false;
    destroy_heap();
    return printf("SUCCESS\n"), true;
}

bool test_validate(void)
{
    printf("HEAP VALIDATE TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    if (heap_validate() != 0)
        return destroy_heap(), false;
    printf("Valid 1...");
    int *ptr = heap_malloc(20 * sizeof(int));
    if (heap_validate() != 0)
        return destroy_heap(), false;
    printf("Valid 2...");
    *(ptr - 2) = 45;
    if (heap_validate() == 0)
        return destroy_heap(), false;
    printf("Valid 3...");
    destroy_heap();
    heap_setup();
    ptr = heap_malloc(20 * sizeof(int));
    *(ptr + 21) = 555;
    if (heap_validate() == 0)
        return destroy_heap(), false;
    printf("Valid 4...");
    return printf("SUCCESS\n"), destroy_heap(), true;
}

bool test_multithreaded_workload(void)
{
    int threadcount = 20;
    printf("MULTITHREADED WORKLOAD TEST Starting...");
    if (heap_setup())
        return printf("heap_setup failed. Test aborted\n"), false;
    printf(" Heap set... ");
    if (heap_validate() != 0)
        return false;
    pthread_t threads[threadcount];
    int results[threadcount];
    int status = 0;
    for (int i = 0; i < threadcount; i++)
        pthread_create(&threads[i], NULL, singular_malloc_test, (void *)&results[i]);
    for (int i = 0; i < threadcount / 3; i++)
        status += heap_validate();
    for (int i = 0; i < threadcount; i++)
        pthread_join(threads[i], (void *)(results + i));
    for (int i = 0; i < 10; i++)
        status += results[i];
    if (!status)
        return printf("TEST SUCCESS!"), destroy_heap(), true;
    return printf("%d multithreaded tests failed :<\n", status), destroy_heap(), false;
}

void *singular_malloc_test(void *nothing)
{
    void *ptr1, *ptr2, *ptr3;
    int *ret = (int *)nothing;
    *ret = 0;
    ptr1 = heap_malloc(sizeof(int) * 1024);
    *ret += (heap_validate() == 0);
    ptr2 = heap_calloc(2048, sizeof(double));
    heap_free(ptr1);
    *ret += (heap_validate() == 0);
    ptr3 = heap_calloc_aligned(500, sizeof(char *));
    ptr1 = heap_malloc(sizeof(void *) * 813);
    *ret += (heap_validate() == 0);
    heap_free(ptr2);
    heap_free(ptr3);
    *ret += (heap_validate() == 0);
    heap_free(ptr1);
    *ret += (heap_validate() == 0);
    return NULL;
}
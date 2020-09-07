#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include "mylloc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include "custom_unistd.h"

bool test_heap_setup(void);

bool test_malloc_and_free(void);

bool test_calloc(void);

bool test_realloc(void);

bool test_malloc_aligned(void);
bool test_calloc_aligned(void);
bool test_realloc_aligned(void);

bool test_get_used_space(void);

bool test_get_largest_used_block_size(void);
bool test_get_used_blocks_count(void);
bool test_get_free_space(void);
bool test_get_largest_free_area(void);
bool test_get_free_gaps_count(void);

bool test_get_pointer_type(const void *const pointer);

bool test_get_data_block_start(const void *pointer);

bool test_get_block_size(const void *const memblock);

bool test_validate(void);

bool test_resize_heap_pages(int pages);

void destroy_heap(void);

void heap_show_short(void);
#endif
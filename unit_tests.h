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

int run_tests(void);

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

bool test_get_pointer_type(void);

bool test_get_data_block_start(void);

bool test_get_block_size(void);

bool test_validate(void);

void destroy_heap(void);

void heap_show_short(void);

void heap_show_short_alignment(void);

bool test_multithreaded_workload(void);

void *singular_malloc_test(void *nothing);
#endif
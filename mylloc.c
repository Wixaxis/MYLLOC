#include "mylloc.h"

void feed(char *msg)
{
#ifdef BUGGED
    feedback(msg);
#else
    return;
#endif
}

void set_fences(_chunk *chunk)
{
    int to_increase = 0;
    to_increase += chunk->fence_left.fens[0] = rand() % 100;
    to_increase += chunk->fence_left.fens[FENCE_SIZE - 1] = rand() % 100;
    to_increase += chunk->fence_right.fens[0] = rand() % 100;
    to_increase += chunk->fence_right.fens[FENCE_SIZE - 1] = rand() % 100;
    heap.fence_sum += to_increase;
}

void *mylloc(size_t size)
{
    if (0 >= size)
        return NULL;
    int to_allocate = 0;
    if (!heap.heap_size)
    {
        to_allocate = PAGE_SIZE * SIZE2PAGES((size + 2 * sizeof(_chunk))); //Calculate how many bytes to give to sbrk
        void *new = sbrk(to_allocate);                                     //allocating to_allocate bytes
        if (BRK_ERR == new)
            return feed("SBRK ERR"), NULL;
        memset(new, 0, to_allocate);                                   //zeroing new memory
        heap.heap_size = to_allocate;                                  //setting heap size to allocated memory
        heap.first_chunk = (_chunk *)new;                              //setting first chunk at beggining of memory
        _chunk *to_give = heap.first_chunk;                            //setting to_give as first chunk
        heap.last_chunk = (char *)to_give + MEM_SIZE2CHUNK_SIZE(size); //setting last chunk as memory right after first chunk
        to_give->mem_size = size;                                      //setting first chunk mem_size to size
        to_give->next_chunk = heap.last_chunk;                         //setting next chunk of first chunk as last chunk
        to_give->prev_chunk = NULL;                                    //setting previous chunk of first chunk as NULL
        to_give->is_free = false;                                      //setting to_give as taken
        set_fences(to_give);                                           //setting fences in first chunk
        set_fences(heap.last_chunk);                                   //setting fences in last chunk
        }
}
/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <stdlib.h>
#include "memory.h"

#ifdef __GLIBC__
#include <malloc.h>
#endif 

int tb_memory_allocated_est;
int tb_memory_allocated;
int tb_memory_free;

int tb_memory_times_allocated;
int tb_memory_times_freed;

#ifdef __GLIBC__
void propagate_memory()
{
    struct mallinfo2 inf = mallinfo2();

    tb_memory_free = inf.fordblks;
    tb_memory_allocated = inf.uordblks;
}
#endif // __GLIBC__

void* tb_malloc(size_t size)
{
    tb_memory_allocated_est += size;
    ++tb_memory_times_allocated;
    return malloc(size);
}

// Nothing....
void* tb_calloc(size_t nmemb, size_t size)
{
    tb_memory_allocated_est = size * nmemb;
    return calloc(nmemb, size);
}

void* tb_realloc(void* ptr, size_t size)
{
    tb_memory_allocated_est = size;
    return realloc(ptr, size);
}

void tb_free(void* ptr)
{
    ++tb_memory_times_freed;
    free(ptr);
}

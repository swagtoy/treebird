/*
 * Treebird - Lightweight frontend for Pleroma
 * Copyright (C) 2022 Nekobit
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <malloc.h>
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
    tb_memory_allocated_est += size * nmemb;
    return calloc(nmemb, size);
}

void* tb_realloc(void* ptr, size_t size)
{
    tb_memory_allocated_est += size;
    return realloc(ptr, size);
}

void tb_free(void* ptr)
{
    ++tb_memory_times_freed;
    free(ptr);
}

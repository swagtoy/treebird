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

/* Everything seen here is info for the super curious, and does not really
   have anything to do with Treebird :-) */

#ifndef TREEBIRD_MEMSTUF_H
#define TREEBIRD_MEMSTUF_H
#include <stdlib.h>

extern int tb_memory_allocated_est;
extern int tb_memory_allocated;
extern int tb_memory_free;

extern int tb_memory_times_allocated;
extern int tb_memory_times_freed;

#if 0
#define tb_malloc malloc
#define tb_calloc calloc
#define tb_realloc realloc
#define tb_free free
#endif // Nothing

void* tb_malloc(size_t size);
void* tb_calloc(size_t nmemb, size_t size);
void* tb_realloc(void* ptr, size_t size);
void tb_free(void* ptr);

#ifdef __GLIBC__
void propagate_memory();
#else
#define propagate_memory() ;
#endif

#endif // TREEBIRD_MEMSTUF_H

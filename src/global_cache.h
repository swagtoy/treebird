/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef GLOBAL_CACHE_H
#define GLOBAL_CACHE_H
#include <mastodont.h>

#if 0
struct global_cache
{
    struct mstdnt_fetch_results tos_html;
    struct mstdnt_fetch_results panel_html;
};

extern struct global_cache g_cache;

void load_instance_info_cache(mastodont_t* api);
void free_instance_info_cache();
#endif

#endif /* GLOBAL_CACHE_H */

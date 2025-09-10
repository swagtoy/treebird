/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#if 0
#include "config.h"
#include "global_cache.h"

struct global_cache g_cache = { {} };

void load_instance_info_cache(mastodont_t* api)
{
    struct mstdnt_args m_args = {
        .url = config_instance_url,
        .token = 0,
        .flags = config_library_flags,
    };
    mstdnt_instance_panel(api, &m_args, NULL, NULL, &(g_cache.panel_html));
    mstdnt_terms_of_service(api, &m_args, NULL, NULL, &(g_cache.tos_html));
}

void free_instance_info_cache()
{
    mstdnt_fetch_results_cleanup(&(g_cache.panel_html));
    mstdnt_fetch_results_cleanup(&(g_cache.tos_html));
}

#endif

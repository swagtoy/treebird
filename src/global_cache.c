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

#include "../config.h"
#include "global_cache.h"

struct global_cache g_cache = { 0 };

void load_instance_info_cache(mastodont_t* api)
{
    struct mstdnt_args m_args = {
        .url = config_instance_url,
        .token = 0,
        .flags = config_library_flags,
    };
    mastodont_instance_panel(api, &m_args, &(g_cache.panel_html));
    mastodont_terms_of_service(api, &m_args, &(g_cache.tos_html));
}

void free_instance_info_cache()
{
    mastodont_fetch_results_cleanup(&(g_cache.panel_html));
    mastodont_fetch_results_cleanup(&(g_cache.tos_html));
}

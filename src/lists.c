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
#include "base_page.h"
#include "../config.h"
#include "account.h"
#include "easprintf.h"
#include "status.h"
#include "lists.h"
#include "string_helpers.h"

// Files
#include "../static/index.chtml"
#include "../static/account.chtml"
#include "../static/list.chtml"
#include "../static/lists.chtml"

char* construct_list(struct mstdnt_list* list, int* size)
{
    char* list_html;
    size_t s = easprintf(&list_html, data_list_html,
                         config_url_prefix,
                         list->id,
                         list->title);
    if (size) *size = s;
    return list_html;
}

static char* construct_list_voidwrap(void* passed, size_t index, int* res)
{
    return construct_list((struct mstdnt_list*)passed + index, res);
}

char* construct_lists(struct mstdnt_list* lists, size_t size, size_t* ret_size)
{
    return construct_func_strings(construct_list_voidwrap, lists, size, ret_size);
}

char* construct_lists_view(char* lists_string, int* size)
{
    char* list_string;
    size_t s = easprintf(&list_string, data_lists_html, lists_string, config_url_prefix);
    if (size) *size = s;
    return list_string;
}

void content_lists(mastodont_t* api, char** data, size_t size)
{
    int cleanup = 0;
    struct mstdnt_list* lists;
    size_t size_list;
    struct mstdnt_storage storage = { 0 };
    char* lists_format;
    char* lists_page = NULL;

    if (mastodont_get_lists(api, &lists, &storage, &size_list))
    {
        lists_format = "An error occured while fetching lists";
    }
    else {
        lists_format = construct_lists(lists, size_list, NULL);
        if (!lists_format)
            lists_format = "Error in malloc!";
        cleanup = 1;
    }

    lists_page = construct_lists_view(lists_format, NULL);

    struct base_page b = {
        .locale = L10N_EN_US,
        .content = lists_page,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    if (cleanup) free(lists_format);
    if (lists_page) free(lists_page);
}

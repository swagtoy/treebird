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
#include "error.h"
#include "status.h"
#include "lists.h"
#include "string_helpers.h"

// Files
#include "../static/account.ctmpl"
#include "../static/list.ctmpl"
#include "../static/lists.ctmpl"

char* construct_list(struct mstdnt_list* list, size_t* size)
{
    struct list_template data = {
        .list = list->title,
        .prefix = config_url_prefix,
        .list_id = list->id
    };
    return tmpl_gen_list(&data, size);
}

static char* construct_list_voidwrap(void* passed, size_t index, size_t* res)
{
    return construct_list((struct mstdnt_list*)passed + index, res);
}

char* construct_lists(struct mstdnt_list* lists, size_t size, size_t* ret_size)
{
    return construct_func_strings(construct_list_voidwrap, lists, size, ret_size);
}

char* construct_lists_view(char* lists_string, size_t* size)
{
    struct lists_template data = {
        .lists = lists_string,
        .prefix = config_url_prefix
    };
    return tmpl_gen_lists(&data, size);
}

void content_lists(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_list* lists;
    size_t size_list = 0;
    struct mstdnt_storage storage = { 0 };
    char* lists_format = NULL;
    char* lists_page = NULL;

    if (mastodont_get_lists(api, &lists, &storage, &size_list))
    {
        lists_page = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        lists_format = construct_lists(lists, size_list, NULL);
        if (!lists_format)
            lists_format = construct_error("No lists", E_ERROR, 1, NULL);
        lists_page = construct_lists_view(lists_format, NULL);
    }

    struct base_page b = {
        .category = BASE_CAT_LISTS,
        .content = lists_page,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    if (lists_format) free(lists_format);
    if (lists_page) free(lists_page);
}

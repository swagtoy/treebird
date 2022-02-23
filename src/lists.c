/*
 * RatFE - Lightweight frontend for Pleroma
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
#include <stdio.h>
#include "base_page.h"
#include "../config.h"
#include "account.h"
#include "easprintf.h"
#include "status.h"
#include "lists.h"

// Files
#include "../static/index.chtml"
#include "../static/account.chtml"
#include "../static/list.chtml"
#include "../static/lists.chtml"

void construct_list(struct mstdnt_list* list, int* size)
{
    char* list_html;
    size_t s = easprintf(&list_html, data_list_html,
                         "");
    if (size) *size = s;
    return list_html;
}

void content_lists(mastodont_t* api, char** data, size_t size)
{
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = data_lists_html,
        .sidebar_right = NULL
    };

    // Output
    render_base_page(&b);

    // Cleanup
}

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

#include "notifications.h"
#include "base_page.h"
#include "string_helpers.h"
#include "easprintf.h"

// Pages
#include "../static/notifications_page.chtml"

static char* construct_notifications_voidwrap(void* passed, size_t index, int* res)
{
    return construct_notifications((struct mstdnt_notification*)passed + index, res);
}

char* construct_notifications(struct mstdnt_notification* notifs, size_t size, size_t* ret_size)
{
    return construct_func_strings(construct_notifications_voidwrap, notifs, size, ret_size);
}

void content_notifications(mastodont_t* api, char** data, size_t data_size)
{
    
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = data_notifications_page_html,
        .sidebar_right = NULL
    };

    // Output
    render_base_page(&b);
}


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
#include "notifications.h"
#include "base_page.h"
#include "string_helpers.h"
#include "easprintf.h"

// Pages
#include "../static/notifications_page.chtml"
#include "../static/notifications.chtml"
#include "../static/notification.chtml"
#include "../static/notification_compact.chtml"

char* construct_notification(struct mstdnt_notification* notif, int* size)
{
    char* notif_html;
    return notif_html;
}

char* construct_notification_compact(struct mstdnt_notification* notif, int* size)
{
    char* notif_html;
    char* notif_stats = NULL;

    if (notif->status)
        easprintf(&notif_stats, "%d - %d - %d",
                  notif->status->replies_count,
                  notif->status->reblogs_count,
                  notif->status->favourites_count);
    

    size_t s = easprintf(&notif_html, data_notification_compact_html,
                         notif->account->avatar,
                         notif->account->display_name,
                         "interacted",
                         notif->status ? notif->status->content : "",
                         notif_stats ? notif_stats : "");

    if (size) *size = s;

    if (notif_stats) free(notif_stats);
    return notif_html;
}

static char* construct_notification_voidwrap(void* passed, size_t index, int* res)
{
    return construct_notification((struct mstdnt_notification*)passed + index, res);
}

static char* construct_notification_compact_voidwrap(void* passed, size_t index, int* res)
{
    return construct_notification_compact((struct mstdnt_notification*)passed + index, res);
}

char* construct_notifications(struct mstdnt_notification* notifs,
                              size_t size,
                              size_t* ret_size)
{
    return construct_func_strings(construct_notification_voidwrap, notifs, size, ret_size);
}

char* construct_notifications_compact(struct mstdnt_notification* notifs,
                                      size_t size,
                                      size_t* ret_size)
{
    return construct_func_strings(construct_notification_compact_voidwrap,
                                  notifs,
                                  size,
                                  ret_size);
}

void content_notifications(mastodont_t* api, char** data, size_t data_size)
{
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = data_notifications_page_html,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, api);
}


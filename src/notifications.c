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

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "notifications.h"
#include "base_page.h"
#include "string_helpers.h"
#include "easprintf.h"
#include "status.h"
#include "error.h"
#include "../config.h"

// Pages
#include "../static/notifications_page.chtml"
#include "../static/notifications.chtml"
#include "../static/notification_action.chtml"
#include "../static/notification.chtml"
#include "../static/notification_compact.chtml"
#include "../static/like_svg.chtml"
#include "../static/repeat_svg.chtml"

char* construct_notification(struct mstdnt_notification* notif, int* size)
{
    char* notif_html;
    int s = 0;

    if (notif->status)
    {
        // Construct status with notification_info
        notif_html = construct_status(notif->status, &s, notif, 0);
    }
    else {
        notif_html = construct_notification_action(notif, &s);
    }

    if (size) *size = s;
    return notif_html;
}

char* construct_notification_action(struct mstdnt_notification* notif, int* size)
{
    char* notif_html;
    int s;

    s = easprintf(&notif_html, data_notification_action_html,
                  notif->account->avatar,
                  notif->account->acct,
                  notif->account->display_name,
                  notification_type_compact_str(notif->type),
                  notification_type_svg(notif->type),
                  config_url_prefix,
                  notif->account->acct,
                  notif->account->acct);

    if (size) *size = s;
    return notif_html;
}

char* construct_notification_compact(struct mstdnt_notification* notif, int* size)
{
    char* notif_html;
    char* notif_stats = NULL;

    const char* type_str = notification_type_compact_str(notif->type);
    const char* type_svg = notification_type_svg(notif->type);

    if (notif->status)
        easprintf(&notif_stats, "%d - %d - %d",
                  notif->status->replies_count,
                  notif->status->reblogs_count,
                  notif->status->favourites_count);

    size_t s = easprintf(&notif_html, data_notification_compact_html,
                         notif->account->avatar,
                         /* If there is an icon, the text doesn't shift up relative to the SVG, this is a hack on the CSS side */
                         strlen(type_svg) == 0 ? "" : "-with-icon",
                         notif->account->acct,
                         notif->account->display_name,
                         type_str,
                         type_svg,
                         /* Might show follower address */
                         notif->type == MSTDNT_NOTIFICATION_FOLLOW ?
                         notif->account->acct :
                         notif->status ? notif->status->content : "",
                         /* end */
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

void content_notifications(struct session* ssn, mastodont_t* api, char** data)
{
    char* page, *notif_html = NULL;
    struct mstdnt_storage storage;
    struct mstdnt_notification* notifs;
    size_t notifs_len;

    if (ssn->cookies.logged_in)
    {
        struct mstdnt_get_notifications_args args = {
            .exclude_types = 0,
            .account_id = NULL,
            .exclude_visibilities = 0,
            .include_types = 0,
            .with_muted = 1,
            .max_id = NULL,
            .min_id = NULL,
            .since_id = NULL,
            .offset = 0,
            .limit = 20,
        };

        if (mastodont_get_notifications(api, &args, &storage, &notifs, &notifs_len) == 0)
        {
            notif_html = construct_notifications(notifs, notifs_len, NULL);
            mstdnt_cleanup_notifications(notifs, notifs_len);
        }
        else
            notif_html = construct_error(storage.error, NULL);

    }
 
    easprintf(&page, data_notifications_page_html,
              notif_html ? notif_html : "Not logged in");
    
    struct base_page b = {
        .category = BASE_CAT_NOTIFICATIONS,
        .locale = L10N_EN_US,
        .content = page,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);
    if (notif_html) free(notif_html);
    if (page) free(page);
}


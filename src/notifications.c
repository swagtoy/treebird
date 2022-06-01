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
#include "navigation.h"
#include "status.h"
#include "error.h"
#include "../config.h"

// Pages
#include "../static/notifications_page.ctmpl"
#include "../static/notifications.ctmpl"
#include "../static/notification_action.ctmpl"
#include "../static/notification.ctmpl"
#include "../static/notification_compact.ctmpl"
#include "../static/like_svg.ctmpl"
#include "../static/repeat_svg.ctmpl"
#include "../static/notifications_embed.ctmpl"

struct notification_args
{
    struct session* ssn;
    mastodont_t* api;
    struct mstdnt_notification* notifs;
};

char* construct_notification(struct session* ssn,
                             mastodont_t* api,
                             struct mstdnt_notification* notif,
                             int* size)
{
    char* notif_html;
    int s = 0;

    if (notif->status)
    {
        // Construct status with notification_info
        notif_html = construct_status(ssn, api, notif->status, &s, notif, NULL, 0);
    }
    else {
        notif_html = construct_notification_action(notif, &s);
    }

    if (size) *size = s;
    return notif_html;
}

char* construct_notification_action(struct mstdnt_notification* notif, int* size)
{
    struct notification_action_template tdata = {
        .avatar = notif->account->avatar,
        .acct = notif->account->acct,
        .display_name = notif->account->display_name,
        .prefix = config_url_prefix,
        .action = notification_type_compact_str(notif->type),
        .notif_svg = notification_type_svg(notif->type)
    };
    return tmpl_gen_notification_action(&tdata, size);
}

char* construct_notification_compact(struct session* ssn,
                                     mastodont_t* api,
                                     struct mstdnt_notification* notif,
                                     int* size)
{
    char* notif_html;
    char* status_format = NULL;
    char* notif_stats = NULL;

    const char* type_str = notification_type_compact_str(notif->type);
    const char* type_svg = notification_type_svg(notif->type);

    if (notif->status)
    {
        if (notif->type == MSTDNT_NOTIFICATION_MENTION)
            notif_stats = construct_interaction_buttons(ssn, notif->status, NULL,
                                                        STATUS_NO_LIKEBOOST | STATUS_NO_DOPAMEME);
        status_format = reformat_status(ssn,
                                        notif->status->content,
                                        notif->status->emojis,
                                        notif->status->emojis_len);
    }

    struct notification_compact_template tdata = {
        .avatar = notif->account->avatar,
        .has_icon = strlen(type_svg) == 0 ? "" : "-with-icon",
        .acct = notif->account->acct,
        .display_name = notif->account->display_name,
        .action = type_str,
        .notif_svg = type_svg,
        /* Might show follower address */
        .content = (notif->type == MSTDNT_NOTIFICATION_FOLLOW ?
                   notif->account->acct : status_format),
        .stats = notif_stats
    };

    notif_html = tmpl_gen_notification_compact(&tdata, size);

    if (status_format &&
        status_format != notif->status->content) free(status_format);
    if (notif_stats) free(notif_stats);
    return notif_html;
}

static char* construct_notification_voidwrap(void* passed, size_t index, int* res)
{
    struct notification_args* args = passed;
    return construct_notification(args->ssn, args->api, args->notifs + index, res);
}

static char* construct_notification_compact_voidwrap(void* passed, size_t index, int* res)
{
    struct notification_args* args = passed;
    return construct_notification_compact(args->ssn, args->api, args->notifs + index, res);
}

char* construct_notifications(struct session* ssn,
                              mastodont_t* api,
                              struct mstdnt_notification* notifs,
                              size_t size,
                              size_t* ret_size)
{
    struct notification_args args = {
        .ssn = ssn,
        .api = api,
        .notifs = notifs
    };
    return construct_func_strings(construct_notification_voidwrap, &args, size, ret_size);
}

char* construct_notifications_compact(struct session* ssn,
                                      mastodont_t* api,
                                      struct mstdnt_notification* notifs,
                                      size_t size,
                                      size_t* ret_size)
{
    struct notification_args args = {
        .ssn = ssn,
        .api = api,
        .notifs = notifs
    };
    return construct_func_strings(construct_notification_compact_voidwrap,
                                  &args,
                                  size,
                                  ret_size);
}

void content_notifications(struct session* ssn, mastodont_t* api, char** data)
{
    char* page, *notif_html = NULL;
    struct mstdnt_storage storage;
    struct mstdnt_notification* notifs;
    size_t notifs_len;
    char* start_id;
    char* navigation_box = NULL;

    if (keystr(ssn->cookies.logged_in))
    {
        struct mstdnt_get_notifications_args args = {
            .exclude_types = 0,
            .account_id = NULL,
            .exclude_visibilities = 0,
            .include_types = 0,
            .with_muted = 1,
            .max_id = keystr(ssn->post.max_id),
            .min_id = keystr(ssn->post.min_id),
            .since_id = NULL,
            .offset = 0,
            .limit = 20,
        };

        if (mastodont_get_notifications(api, &args, &storage, &notifs, &notifs_len) == 0)
        {
            notif_html = construct_notifications(ssn, api, notifs, notifs_len, NULL);
            start_id = keystr(ssn->post.start_id) ? keystr(ssn->post.start_id) : notifs[0].id;
            navigation_box = construct_navigation_box(start_id,
                                                      notifs[0].id,
                                                      notifs[notifs_len-1].id,
                                                      NULL);
            mstdnt_cleanup_notifications(notifs, notifs_len);}
                
        else
            notif_html = construct_error(storage.error, E_NOTICE, 1, NULL);

    }

    struct notifications_page_template tdata = {
        .notifications = notif_html,
        .navigation = navigation_box
    };

    page = tmpl_gen_notifications_page(&tdata, NULL);
    
    struct base_page b = {
        .category = BASE_CAT_NOTIFICATIONS,
        .locale = L10N_EN_US,
        .content = page,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);
    if (notif_html) free(notif_html);
    if (navigation_box) free(navigation_box);
    if (page) free(page);
}

void content_notifications_compact(struct session* ssn, mastodont_t* api, char** data)
{
    char* page, *notif_html = NULL;
    struct mstdnt_storage storage;
    struct mstdnt_notification* notifs;
    size_t notifs_len;
    char* start_id;
    char* navigation_box = NULL;

    if (keystr(ssn->cookies.logged_in))
    {
        struct mstdnt_get_notifications_args args = {
            .exclude_types = 0,
            .account_id = NULL,
            .exclude_visibilities = 0,
            .include_types = 0,
            .with_muted = 1,
            .max_id = keystr(ssn->post.max_id),
            .min_id = keystr(ssn->post.min_id),
            .since_id = NULL,
            .offset = 0,
            .limit = 20,
        };

        if (mastodont_get_notifications(api, &args, &storage, &notifs, &notifs_len) == 0)
        {
            notif_html = construct_notifications_compact(ssn, api, notifs, notifs_len, NULL);
            start_id = keystr(ssn->post.start_id) ? keystr(ssn->post.start_id) : notifs[0].id;
            navigation_box = construct_navigation_box(start_id,
                                                      notifs[0].id,
                                                      notifs[notifs_len-1].id,
                                                      NULL);
            mstdnt_cleanup_notifications(notifs, notifs_len);}
                
        else
            notif_html = construct_error(storage.error, E_NOTICE, 1, NULL);

    }

    size_t len;
    struct notifications_embed_template tdata = {
        .theme = ssn->config.theme,
        .theme_var = ssn->config.themeclr ? "-dark" : NULL,
        .notifications = notif_html,
        .navigation_box = navigation_box
    };

    page = tmpl_gen_notifications_embed(&tdata, &len);

    render_html(NULL, page, len);

    if (notif_html) free(notif_html);
    if (navigation_box) free(navigation_box);
    if (page) free(page);
}


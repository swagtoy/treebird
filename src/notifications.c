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
#include "string.h"
#include "helpers.h"
#include "notifications.h"
#include "base_page.h"
#include "string_helpers.h"
#include "easprintf.h"
#include "navigation.h"
#include "http.h"
#include "status.h"
#include "error.h"
#include "emoji.h"
#include "account.h"
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
                             size_t* size)
{
    char* notif_html;

    if (notif->status)
    {
        // Construct status with notification_info
        notif_html = construct_status(ssn, api, notif->status, size, notif, NULL, 0);
    }
    else {
        notif_html = construct_notification_action(notif, size);
    }

    return notif_html;
}

char* construct_notification_action(struct mstdnt_notification* notif, size_t* size)
{
    char* res;
    char* serialized_display_name = sanitize_html(notif->account->display_name);
    char* display_name = emojify(serialized_display_name,
                                 notif->account->emojis,
                                 notif->account->emojis_len);
    struct notification_action_template tdata = {
        .avatar = notif->account->avatar,
        .acct = notif->account->acct,
        .display_name = display_name,
        .prefix = config_url_prefix,
        .action = notification_type_compact_str(notif->type),
        .notif_svg = notification_type_svg(notif->type)
    };
    res = tmpl_gen_notification_action(&tdata, size);
    /* // Cleanup */
    if (display_name != notif->account->display_name &&
        display_name != serialized_display_name)
        free(display_name);
    if (serialized_display_name != notif->account->display_name)
        free(serialized_display_name);
    return res;
}

char* construct_notification_compact(struct session* ssn,
                                     mastodont_t* api,
                                     struct mstdnt_notification* notif,
                                     size_t* size)
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

    char* serialized_display_name = sanitize_html(notif->account->display_name);
    char* display_name = emojify(serialized_display_name,
                                 notif->account->emojis,
                                 notif->account->emojis_len);
    struct notification_compact_template tdata = {
        .avatar = notif->account->avatar,
        .has_icon = strlen(type_svg) == 0 ? "" : "-with-icon",
        .acct = notif->account->acct,
        .display_name = display_name,
        .action = type_str,
        .notif_svg = type_svg,
        .is_status = (notif->type == MSTDNT_NOTIFICATION_STATUS ||
                      notif->type == MSTDNT_NOTIFICATION_MENTION ? "is-mention" : NULL),
        /* Might show follower address */
        .content = (notif->type == MSTDNT_NOTIFICATION_FOLLOW ?
                   notif->account->acct : status_format),
        .stats = notif_stats
    };

    notif_html = tmpl_gen_notification_compact(&tdata, size);

    if (status_format &&
        status_format != notif->status->content) free(status_format);
    if (notif_stats) free(notif_stats);
    if (serialized_display_name != notif->account->display_name)
        free(serialized_display_name);
    if (display_name != notif->account->display_name &&
        display_name != serialized_display_name)
        free(display_name);
    return notif_html;
}

static char* construct_notification_voidwrap(void* passed, size_t index, size_t* res)
{
    struct notification_args* args = passed;
    return construct_notification(args->ssn, args->api, args->notifs + index, res);
}

static char* construct_notification_compact_voidwrap(void* passed, size_t index, size_t* res)
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

void content_notifications(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_notification* notifs = NULL;
    size_t notifs_len = 0;
    
    struct mstdnt_notifications_args args = {
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

    if (keystr(ssn->cookies.logged_in))
        mastodont_get_notifications(api, &m_args, &args, &storage, &notifs, &notifs_len);

    // Call perl
    perl_lock();
    dSP;
    ENTER;
    SAVETMPS;
    PUSHMARK(SP);
    
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (notifs)
        XPUSHs(newRV_noinc((SV*)perlify_notifications(notifs, notifs_len)));
    
    // ARGS
    PUTBACK;
    call_pv("notifications::content_notifications", G_SCALAR);
    SPAGAIN;

    // Duplicate so we can free the TMPs
    char* dup = savesharedsvpv(POPs);
    PUTBACK;
    FREETMPS;
    LEAVE;
    perl_unlock();
    
    struct base_page b = {
        .category = BASE_CAT_NOTIFICATIONS,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);
    mastodont_storage_cleanup(&storage);
    Safefree(dup);
}

void content_notifications_compact(PATH_ARGS)
{
    char* theme_str = NULL;
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* page, *notif_html = NULL;
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_notification* notifs = NULL;
    size_t notifs_len = 0;
    char* start_id = NULL;
    char* navigation_box = NULL;

    if (keystr(ssn->cookies.logged_in))
    {
        struct mstdnt_notifications_args args = {
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

        if (mastodont_get_notifications(api,
                                        &m_args,
                                        &args,
                                        &storage,
                                        &notifs,
                                        &notifs_len) == 0)
        {
            if (notifs && notifs_len)
            {
                notif_html = construct_notifications_compact(ssn, api, notifs, notifs_len, NULL);
                start_id = keystr(ssn->post.start_id) ? keystr(ssn->post.start_id) : notifs[0].id;
                navigation_box = construct_navigation_box(start_id,
                                                          notifs[0].id,
                                                          notifs[notifs_len-1].id,
                                                          NULL);
                mstdnt_cleanup_notifications(notifs, notifs_len);
            }
            else
                notif_html = construct_error("No notifications", E_NOTICE, 1, NULL);
        }   
        else
            notif_html = construct_error(storage.error, E_ERROR, 1, NULL);

    }

    // Set theme
    if (ssn->config.theme && !(strcmp(ssn->config.theme, "treebird") == 0 &&
          ssn->config.themeclr == 0))
    {
        easprintf(&theme_str, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s%s.css\">",
                  ssn->config.theme,
                  ssn->config.themeclr ? "-dark" : "");
    }
    
    size_t len;
    struct notifications_embed_template tdata = {
        .theme_str = theme_str,
        .notifications = notif_html,
        .navigation_box = navigation_box
    };

    page = tmpl_gen_notifications_embed(&tdata, &len);

    send_result(req, NULL, NULL, page, len);

    mastodont_storage_cleanup(&storage);
    free(notif_html);
    free(navigation_box);
    free(page);
    free(theme_str);
}

void content_notifications_clear(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };

    mastodont_notifications_clear(api, &m_args, &storage);

    mastodont_storage_cleanup(&storage);
    redirect(req, REDIRECT_303, referer);
}

// Converts it into a perl struct
HV* perlify_notification(struct mstdnt_notification* notif)
{
    if (!notif) return NULL;
    HV* notif_hv = newHV();

    hvstores_str(notif_hv, "id", notif->id);
    hvstores_str(notif_hv, "created_at", notif->created_at);
    hvstores_str(notif_hv, "emoji", notif->emoji);
    hvstores_str(notif_hv, "type", mstdnt_notification_t_to_str(notif->type));
    hvstores_ref(notif_hv, "account", perlify_account(notif->account));
    hvstores_ref(notif_hv, "status", perlify_status(notif->status));
    
    return notif_hv;
}

// The same as above, but for multiple
AV* perlify_notifications(struct mstdnt_notification* notifs, size_t len)
{
    if (!(notifs && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_notification(notifs + i)));
    }

    return av;
}

void api_notifications(PATH_ARGS)
{
    send_result(req, NULL, "application/json", "{\"status\":0}", 0);

}

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
#include "http.h"
#include "status.h"
#include "error.h"
#include "emoji.h"
#include "account.h"
#include "../config.h"

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

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (notifs)
        mXPUSHs(newRV_noinc((SV*)perlify_notifications(notifs, notifs_len)));
    
    // ARGS
    PERL_STACK_SCALAR_CALL("notifications::content_notifications");

    // Duplicate so we can free the TMPs
    char* dup = PERL_GET_STACK_EXIT;
    
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
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* page;
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_notification* notifs = NULL;
    size_t notifs_len = 0;

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

        mastodont_get_notifications(api, &m_args, &args, &storage, &notifs, &notifs_len);
    }

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    mXPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (notifs)
        mXPUSHs(newRV_noinc((SV*)perlify_notifications(notifs, notifs_len)));

    PERL_STACK_SCALAR_CALL("notifications::embed_notifications");

    page = PERL_GET_STACK_EXIT;

    send_result(req, NULL, NULL, page, 0);

    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_notifications(notifs, notifs_len);
    Safefree(page);
}

void content_notifications_clear(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };

    if (data)
    {
        mastodont_notification_dismiss(api, &m_args, &storage, data[0]);
    }
    else {
        mastodont_notifications_clear(api, &m_args, &storage);
    }

    mastodont_storage_cleanup(&storage);
    redirect(req, REDIRECT_303, referer);
}

void content_notifications_read(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };

    if (data)
    {
        struct mstdnt_notifications_args args = { .id = data[0] };
        mastodont_notifications_read(api, &m_args, &args, &storage, NULL);
    }
    else {
        struct mstdnt_notifications_args args = { .max_id = keystr(ssn->post.max_id) };
        mastodont_notifications_read(api, &m_args, &args, &storage, NULL);
    }

    mastodont_storage_cleanup(&storage);
    redirect(req, REDIRECT_303, referer);
}

// Converts it into a perl struct
static HV* perlify_notification_pleroma(struct mstdnt_notification_pleroma* notif)
{
    if (!notif) return NULL;
    HV* notif_pl_hv = newHV();

    hvstores_int(notif_pl_hv, "is_muted", notif->is_muted);
    hvstores_int(notif_pl_hv, "is_seen", notif->is_seen);
    
    return notif_pl_hv;
}


// Converts it into a perl struct
HV* perlify_notification(const struct mstdnt_notification* const notif)
{
    if (!notif) return NULL;
    HV* notif_hv = newHV();

    hvstores_str(notif_hv, "id", notif->id);
    hvstores_int(notif_hv, "created_at", notif->created_at);
    hvstores_str(notif_hv, "emoji", notif->emoji);
    hvstores_str(notif_hv, "type", mstdnt_notification_t_to_str(notif->type));
    hvstores_ref(notif_hv, "account", perlify_account(notif->account));
    hvstores_ref(notif_hv, "pleroma", perlify_notification_pleroma(notif->pleroma));
    hvstores_ref(notif_hv, "status", perlify_status(notif->status));
    
    return notif_hv;
}

PERLIFY_MULTI(notification, notifications, mstdnt_notification)

void api_notifications(PATH_ARGS)
{
    send_result(req, NULL, "application/json", "{\"status\":0}", 0);
}

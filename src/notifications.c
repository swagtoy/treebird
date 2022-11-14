/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
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

int content_notifications(PATH_ARGS)
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
        mstdnt_get_notifications(api, &m_args, NULL, NULL, &args, &storage, &notifs, &notifs_len);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    mXPUSHs(newRV_inc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));
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
    mstdnt_storage_cleanup(&storage);
    mstdnt_cleanup_notifications(notifs, notifs_len);
    tb_free(dup);
}

int content_notifications_compact(PATH_ARGS)
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

        mstdnt_get_notifications(api, &m_args, NULL, NULL, &args, &storage, &notifs, &notifs_len);
    }

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    mXPUSHs(newRV_noinc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));
    if (notifs)
        mXPUSHs(newRV_noinc((SV*)perlify_notifications(notifs, notifs_len)));

    PERL_STACK_SCALAR_CALL("notifications::embed_notifications");

    page = PERL_GET_STACK_EXIT;

    send_result(req, NULL, NULL, page, 0);

    mstdnt_storage_cleanup(&storage);
    mstdnt_cleanup_notifications(notifs, notifs_len);
    tb_free(page);
}

int content_notifications_clear(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };

    if (data)
    {
        mstdnt_notification_dismiss(api, &m_args, NULL, NULL, &storage, data[0]);
    }
    else {
        mstdnt_notifications_clear(api, &m_args, NULL, NULL, &storage);
    }

    mstdnt_storage_cleanup(&storage);
    redirect(req, REDIRECT_303, referer);
}

int content_notifications_read(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };

    if (data)
    {
        struct mstdnt_notifications_args args = { .id = data[0] };
        mstdnt_notifications_read(api, &m_args, NULL, NULL, &args, &storage, NULL);
    }
    else {
        struct mstdnt_notifications_args args = { .max_id = keystr(ssn->post.max_id) };
        mstdnt_notifications_read(api, &m_args, NULL, NULL, &args, &storage, NULL);
    }

    mstdnt_storage_cleanup(&storage);
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

int api_notifications(PATH_ARGS)
{
    send_result(req, NULL, "application/json", "{\"status\":0}", 0);
}

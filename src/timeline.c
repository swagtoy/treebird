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

#include "global_perl.h"
#include "timeline.h"
#include <stdlib.h>
#include "helpers.h"

#include "base_page.h"
#include "../config.h"
#include "index.h"
#include "status.h"
#include "easprintf.h"
#include "query.h"
#include "error.h"
#include "string_helpers.h"
#include "types.h"

void content_timeline(REQUEST_T req,
                      struct session* ssn,
                      mastodont_t* api,
                      struct mstdnt_storage* storage,
                      struct mstdnt_status* statuses,
                      size_t statuses_len,
                      enum base_category cat,
                      char* header_text,
                      int show_post_box,
                      int fake_timeline)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    mXPUSHs(newRV_inc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));
    
    if (statuses)
        mXPUSHs(newRV_noinc((SV*)perlify_statuses(statuses, statuses_len)));
    else ARG_UNDEFINED();

    if (header_text)
        mXPUSHs(newSVpv(header_text, 0));
    else ARG_UNDEFINED();

    mXPUSHi(show_post_box);
    mXPUSHi(fake_timeline);

    PERL_STACK_SCALAR_CALL("timeline::content_timeline");

    // Duplicate to free temps
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = cat,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);

    // Cleanup
    /* mstdnt_storage_cleanup(storage); */
    /* mstdnt_cleanup_statuses(statuses, statuses_len); */
    tb_free(dup);
}

int tl_home(REQUEST_T req, struct session* ssn, mastodont_t* api, int local)
{
    struct mstdnt_args m_args = { 0 };
    set_mstdnt_args(&m_args, ssn);

    struct mstdnt_timeline_args args = {
        .with_muted = MSTDNT_TRUE,
        .local = local,
        // Converts to `enum mstdnt_reply_visibility' nicely
        .reply_visibility = (ssn->post.replies_only.is_set ?
                             keyint(ssn->post.replies_only) : 0),
        .only_media = (ssn->post.only_media.is_set ?
                       keyint(ssn->post.only_media) : 0),
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = 0,
        .limit = 20,
        .remote = MSTDNT_BOOL_UNSET,
    };
    
    try_post_status(ssn, api);

    
    return mstdnt_timeline_home(api, &m_args, NULL, NULL, &args);

//    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_HOME, NULL, 1, 0);
}

int tl_direct(REQUEST_T req, struct session* ssn, mastodont_t* api)
{
    struct mstdnt_args m_args = { 0 };
    set_mstdnt_args(&m_args, ssn);

    struct mstdnt_timeline_args args = {
        .with_muted = 1,
        .max_id = keystr(ssn->post.max_id),
        // Converts to `enum mstdnt_reply_visibility' nicely
        .reply_visibility = (ssn->post.replies_only.is_set ?
                             keyint(ssn->post.replies_only) : 0),
        .only_media = (ssn->post.only_media.is_set ?
                       keyint(ssn->post.only_media) : 0),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };
    
    try_post_status(ssn, api);
    
    return mstdnt_timeline_direct(api, &m_args, NULL, NULL, &args);

//    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_DIRECT, "Direct", 0, 0);
}

// Callback: tl_public
static void request_cb_tl_public(mstdnt_request_cb_data* cb_data, void* tbargs)
{
    struct mstdnt_statuses* statuses = MSTDNT_CB_DATA(cb_data);
    DESTRUCT_TB_ARGS(tbargs);

    PUT(":worm: wow!");

    FCGX_Finish_r(req);
    free(req);

    //content_timeline(req, ssn, api, cb_data->storage, statuses->statuses, statuses->len, 0, NULL, 1, 0);
    mstdnt_request_cb_cleanup(cb_data);
}

int tl_public(REQUEST_T req, struct session* ssn, mastodont_t* api, int local, enum base_category cat)
{
    struct mstdnt_args m_args = { 0 };
    set_mstdnt_args(&m_args, ssn);

    struct mstdnt_timeline_args args = {
        .with_muted = MSTDNT_TRUE,
        .local = local ? MSTDNT_TRUE : MSTDNT_FALSE,
        .remote = 0,
        // Converts to `enum mstdnt_reply_visibility' nicely
        .reply_visibility = (ssn->post.replies_only.is_set ?
                             keyint(ssn->post.replies_only) : 0),
        .only_media = (ssn->post.only_media.is_set ?
                       keyint(ssn->post.only_media) : 0),
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .limit = 20
    };

    try_post_status(ssn, api);

    struct request_args* cb_args =
        request_args_create(req, ssn, api, NULL);
    
    return mstdnt_timeline_public(api, &m_args, request_cb_tl_public, cb_args, &args);
}

int tl_list(REQUEST_T req, struct session* ssn, mastodont_t* api, char* list_id)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    struct mstdnt_timeline_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        // Converts to `enum mstdnt_reply_visibility' nicely
        .reply_visibility = (ssn->post.replies_only.is_set ?
                             keyint(ssn->post.replies_only) : 0),
        .only_media = (ssn->post.only_media.is_set ?
                       keyint(ssn->post.only_media) : 0),
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };

    try_post_status(ssn, api);
    
    return mstdnt_timeline_list(api, &m_args, NULL, NULL, list_id, &args);

    //content_timeline(req, ssn, api, BASE_CAT_LISTS, "List timeline", 0, 0);
}


int tl_tag(REQUEST_T req, struct session* ssn, mastodont_t* api, char* tag_id)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    struct mstdnt_timeline_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        // Converts to `enum mstdnt_reply_visibility' nicely
        .reply_visibility = (ssn->post.replies_only.is_set ?
                             keyint(ssn->post.replies_only) : 0),
        .only_media = (ssn->post.only_media.is_set ?
                       keyint(ssn->post.only_media) : 0),
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };

    return mstdnt_timeline_tag(api, &m_args, NULL, NULL, tag_id, &args);

    //content_timeline(req, ssn, api, BASE_CAT_NONE, header, 0, 0);
}

int content_tl_home(PATH_ARGS)
{
    if (keystr(ssn->cookies.logged_in))
        return tl_home(req, ssn, api, 0);
    else
        return content_tl_federated(req, ssn, api, data);
}

int content_tl_direct(PATH_ARGS)
{
    (void)data;
    return tl_direct(req, ssn, api);
}

int content_tl_federated(PATH_ARGS)
{
    (void)data;
    return tl_public(req, ssn, api, 0, BASE_CAT_FEDERATED);
}

int content_tl_local(PATH_ARGS)
{
    (void)data;
    return tl_public(req, ssn, api, 1, BASE_CAT_LOCAL);
}

int content_tl_list(PATH_ARGS)
{
    return tl_list(req, ssn, api, data[0]);
}

int content_tl_tag(PATH_ARGS)
{
    return tl_tag(req, ssn, api, data[0]);
}

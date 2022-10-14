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

void content_timeline(REQUEST_T req,
                      struct session* ssn,
                      mstdnt_t* api,
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
    mstdnt_storage_cleanup(storage);
    mstdnt_cleanup_statuses(statuses, statuses_len);
    Safefree(dup);
}

void tl_home(REQUEST_T req, struct session* ssn, mstdnt_t* api, int local)
{
    struct mstdnt_args m_args = { 0 };
    set_mstdnt_args(&m_args, ssn);
    size_t statuses_len = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };

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
    
    mstdnt_timeline_home(api, &m_args, &args, &storage, &statuses, &statuses_len);

    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_HOME, NULL, 1, 0);
}

void tl_direct(REQUEST_T req, struct session* ssn, mstdnt_t* api)
{
    struct mstdnt_args m_args = { 0 };
    set_mstdnt_args(&m_args, ssn);
    size_t statuses_len = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };

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
    
    mstdnt_timeline_direct(api, &m_args, &args, &storage, &statuses, &statuses_len);

    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_DIRECT, "Direct", 0, 0);
}

void tl_public(REQUEST_T req, struct session* ssn, mstdnt_t* api, int local, enum base_category cat)
{
    struct mstdnt_args m_args = { 0 };
    set_mstdnt_args(&m_args, ssn);
    size_t statuses_len = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };

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

    mstdnt_timeline_public(api, &m_args, &args, &storage, &statuses, &statuses_len);

    content_timeline(req, ssn, api, &storage, statuses, statuses_len, cat, NULL, 1, 0);
}

void tl_list(REQUEST_T req, struct session* ssn, mstdnt_t* api, char* list_id)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    size_t statuses_len = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };

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
    
    mstdnt_timeline_list(api, &m_args, list_id, &args, &storage, &statuses, &statuses_len);

    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_LISTS, "List timeline", 0, 0);
}


void tl_tag(REQUEST_T req, struct session* ssn, mstdnt_t* api, char* tag_id)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* header;
    size_t statuses_len = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };

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

    mstdnt_timeline_tag(api, &m_args, tag_id, &args, &storage, &statuses, &statuses_len);

    easprintf(&header, "Hashtag - #%s", tag_id);

    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_NONE, header, 0, 0);
    free(header);
}

void content_tl_home(PATH_ARGS)
{
    if (keystr(ssn->cookies.logged_in))
        tl_home(req, ssn, api, 0);
    else
        content_tl_federated(req, ssn, api, data);
}

void content_tl_direct(PATH_ARGS)
{
    (void)data;
    tl_direct(req, ssn, api);
}

void content_tl_federated(PATH_ARGS)
{
    (void)data;
    tl_public(req, ssn, api, 0, BASE_CAT_FEDERATED);
}

void content_tl_local(PATH_ARGS)
{
    (void)data;
    tl_public(req, ssn, api, 1, BASE_CAT_LOCAL);
}

void content_tl_list(PATH_ARGS)
{
    tl_list(req, ssn, api, data[0]);
}

void content_tl_tag(PATH_ARGS)
{
    tl_tag(req, ssn, api, data[0]);
}

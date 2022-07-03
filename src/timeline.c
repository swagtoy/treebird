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

#include "timeline.h"
#include <stdlib.h>
#include "helpers.h"
#include "base_page.h"
#include "../config.h"
#include "index.h"
#include "status.h"
#include "easprintf.h"
#include "reply.h"
#include "navigation.h"
#include "query.h"
#include "error.h"
#include "string_helpers.h"

#include "../static/timeline_options.ctmpl"
#include "../static/navigation.ctmpl"

void content_timeline(struct session* ssn,
                      mastodont_t* api,
                      struct mstdnt_storage* storage,
                      struct mstdnt_status* statuses,
                      size_t statuses_len,
                      enum base_category cat,
                      char* header_text,
                      int show_post_box)
{
    size_t statuses_html_count = 0;
    char* status_format = NULL,
        * header = NULL,
        * post_box = NULL,
        * navigation_box = NULL,
        * timeline_options,
        * output = NULL,
        * start_id;

    if (storage->error)
        status_format = construct_error(storage->error, E_ERROR, 1, NULL);
    else
    {
        // Construct statuses into HTML
        status_format = construct_statuses(ssn, api, statuses, statuses_len, NULL, &statuses_html_count);
        if (!status_format)
            status_format = construct_error("No statuses", E_NOTICE, 1, NULL);
    }

    // Create post box
    if (show_post_box)
        post_box = construct_post_box(NULL, "", NULL);

    if (statuses)
    {
        // If not set, set it
        start_id = keystr(ssn->post.start_id) ? keystr(ssn->post.start_id) : statuses[0].id;
        navigation_box = construct_navigation_box(start_id,
                                                  statuses[0].id,
                                                  statuses[statuses_len-1].id,
                                                  NULL);
    }

    // Create timeline options/menubar
    struct timeline_options_template todata = {
        .only_media = "Only media?",
        .replies = "Replies?",
        .only_media_active = keyint(ssn->post.only_media) ? "checked" : NULL,
    };

    timeline_options = tmpl_gen_timeline_options(&todata, NULL);

    // Display a header bar, usually customized for specific pages
    if (header_text)
    {
        easprintf(&header, "<div class=\"simple-page simple-page-header\"><h1>%s</h1></div>",
                  header_text);
    }
    
    easprintf(&output, "%s%s%s%s%s",
              STR_NULL_EMPTY(header),
              STR_NULL_EMPTY(post_box),
              STR_NULL_EMPTY(timeline_options),
              STR_NULL_EMPTY(status_format),
              STR_NULL_EMPTY(navigation_box));

    struct base_page b = {
        .category = cat,
        .content = output,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(storage);
    mstdnt_cleanup_statuses(statuses, statuses_len);
    free(status_format);
    free(post_box);
    free(header);
    free(timeline_options);
    free(navigation_box);
    free(output);
}

void tl_home(struct session* ssn, mastodont_t* api, int local)
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
    
    mastodont_timeline_home(api, &m_args, &args, &storage, &statuses, &statuses_len);

    content_timeline(ssn, api, &storage, statuses, statuses_len, BASE_CAT_HOME, NULL, 1);
}

void tl_direct(struct session* ssn, mastodont_t* api)
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
    
    mastodont_timeline_direct(api, &m_args, &args, &storage, &statuses, &statuses_len);

    content_timeline(ssn, api, &storage, statuses, statuses_len, BASE_CAT_DIRECT, "Direct", 0);
}

void tl_public(struct session* ssn, mastodont_t* api, int local, enum base_category cat)
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

    mastodont_timeline_public(api, &m_args, &args, &storage, &statuses, &statuses_len);

    content_timeline(ssn, api, &storage, statuses, statuses_len, cat, NULL, 1);
}

void tl_list(struct session* ssn, mastodont_t* api, char* list_id)
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
    
    mastodont_timeline_list(api, &m_args, list_id, &args, &storage, &statuses, &statuses_len);

    content_timeline(ssn, api, &storage, statuses, statuses_len, BASE_CAT_LISTS, "List timeline", 0);
}


void tl_tag(struct session* ssn, mastodont_t* api, char* tag_id)
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

    mastodont_timeline_tag(api, &m_args, tag_id, &args, &storage, &statuses, &statuses_len);

    easprintf(&header, "Hashtag - #%s", tag_id);

    content_timeline(ssn, api, &storage, statuses, statuses_len, BASE_CAT_NONE, header, 0);
    free(header);
}

void content_tl_home(struct session* ssn, mastodont_t* api, char** data)
{
    if (keystr(ssn->cookies.logged_in))
        tl_home(ssn, api, 0);
    else
        content_tl_federated(ssn, api, data);
}

void content_tl_direct(struct session* ssn, mastodont_t* api, char** data)
{
    (void)data;
    tl_direct(ssn, api);
}

void content_tl_federated(struct session* ssn, mastodont_t* api, char** data)
{
    (void)data;
    tl_public(ssn, api, 0, BASE_CAT_FEDERATED);
}

void content_tl_local(struct session* ssn, mastodont_t* api, char** data)
{
    (void)data;
    tl_public(ssn, api, 1, BASE_CAT_LOCAL);
}

void content_tl_list(struct session* ssn, mastodont_t* api, char** data)
{
    tl_list(ssn, api, data[0]);
}

void content_tl_tag(struct session* ssn, mastodont_t* api, char** data)
{
    tl_tag(ssn, api, data[0]);
}

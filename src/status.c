/*
 * RatFE - Lightweight frontend for Pleroma
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
#include <string.h>
#include "base_page.h"
#include "status.h"
#include "easprintf.h"
#include "query.h"
#include "cookie.h"
#include "string_helpers.h"
#include "../config.h"

// Pages
#include "../static/status.chtml"
#include "../static/post.chtml"

#define ID_REPLY_SIZE 256
#define ID_RESPONSE "<input type=\"hidden\" name=\"replyid\" value=\"%s\">"

int try_post_status(mastodont_t* api)
{
    if (!post.content) return 1;

    struct mstdnt_storage storage;

    // Cookie copy and read
    struct mstdnt_create_status_args args = {
        .content_type = "text/plain",
        .expires_in = 0,
        .in_reply_to_conversation_id = NULL,
        .in_reply_to_id = post.replyid,
        .language = NULL,
        .media_ids = NULL,
        .poll = NULL,
        .preview = 0,
        .scheduled_at = NULL,
        .sensitive = 0,
        .spoiler_text = NULL,
        .status = post.content,
        .visibility = "public",
    };
    
    mastodont_create_status(api, &args, &storage);

    // TODO cleanup when errors are properly implemented
    mastodont_storage_cleanup(&storage);
    
    return 0;
}

int try_interact_status(mastodont_t* api, char* id)
{
    struct mstdnt_storage storage;
    if (!(post.itype && id)) return 1;

    // Pretty up the type
    if (strcmp(post.itype, "like") == 0)
    {
        mastodont_favourite_status(api,
                                   id,
                                   &storage);
        // TODO Cleanup when errors handled
        // mastodont_storage_cleanup(&storage);
    }

    return 0;
}

char* construct_post_box(char* reply_id,
                         char* default_content,
                         int* size)
{
    char* reply_html;
    char id_reply[ID_REPLY_SIZE];
    
    // Put hidden post request
    snprintf(id_reply, ID_REPLY_SIZE, ID_RESPONSE, reply_id);

    // Construct box
    size_t s = easprintf(&reply_html, data_post_html, reply_id ? id_reply : "",
                         default_content);
    if (size) *size = s;
    return reply_html;
}

char* construct_status(struct mstdnt_status* status, int* size)
{
    char* stat_html;
    size_t s = easprintf(&stat_html, data_status_html,
                         status->account.avatar,
                         status->account.display_name, /* Username */
                         config_url_prefix,
                         status->account.acct,
                         
                         status->account.acct, /* Account */
                         "Public", /* visibility */
                         status->content,
                         config_url_prefix,
                         status->id,
                         config_url_prefix,
                         status->id,
                         status->reblogged ? "nobutton-active" : "",
                         config_url_prefix,
                         status->id,
                         status->favourited ? "nobutton-active" : "",
                         config_url_prefix,
                         status->id,
                         config_url_prefix,
                         status->id);
    if (size) *size = s;
    return stat_html;
}

static char* construct_status_voidwrap(void* passed, size_t index, int* res)
{
    return construct_status((struct mstdnt_status*)passed + index, res);
}

char* construct_statuses(struct mstdnt_status* statuses, size_t size, size_t* ret_size)
{
    return construct_func_strings(construct_status_voidwrap, statuses, size, ret_size);
}

void status_interact(mastodont_t* api, char** data, size_t data_size)
{
    char* referer = getenv("HTTP_REFERER");
    
    try_interact_status(api, data[0]);
    
    printf("Location: %s\r\n\r\nRedirecting...",
           referer ? referer : "/");
}

void status_view(mastodont_t* api, char** data, size_t data_size)
{
    content_status(api, data, data_size, 0);
}

void status_reply(mastodont_t* api, char** data, size_t data_size)
{
    content_status(api, data, data_size, 1);
}

void content_status(mastodont_t* api, char** data, size_t data_size, int is_reply)
{
    char* output;
    // Status context
    struct mstdnt_storage storage, status_storage;
    struct mstdnt_status* statuses_before, *statuses_after, status;
    size_t stat_before_len, stat_after_len;
    char* before_html = NULL, *stat_html = NULL, *after_html = NULL, *stat_reply;

    try_post_status(api);
    
#ifdef _TEST_
#include "test/status_test.h"
#else
    // Get information
    mastodont_status_context(api, data[0], &storage, &statuses_before, &statuses_after,
                             &stat_before_len, &stat_after_len);
    mastodont_view_status(api, data[0], &status_storage, &status);

    // Before...
    before_html = construct_statuses(statuses_before, stat_before_len, NULL);

    // Current status
    stat_html = construct_status(&status, NULL);
    if (is_reply) stat_reply = construct_post_box(data[0], "", NULL);

    // After...
    after_html = construct_statuses(statuses_after, stat_after_len, NULL);
#endif

    easprintf(&output, "%s%s%s%s",
              before_html ? before_html : "",
              stat_html ? stat_html : "",
              stat_reply ? stat_reply : "",
              after_html ? after_html : "");
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_right = NULL
    };

    // Output
    render_base_page(&b);

    // Cleanup
    if (before_html) free(before_html);
    if (stat_html) free(stat_html);
    if (after_html) free(after_html);
    if (output) free(output);
    if (is_reply) free(stat_reply);
    mastodont_storage_cleanup(&storage);
}

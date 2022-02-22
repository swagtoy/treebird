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
#include "../static/status.chtml"

int try_post_status(mastodont_t* api)
{
    if (!post.content) return 1;

    struct mstdnt_storage storage;

    // Cookie copy and read
    struct mstdnt_create_status_args args = {
        .content_type = "text/plain",
        .expires_in = 0,
        .in_reply_to_conversation_id = NULL,
        .in_reply_to_id = NULL,
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

int try_interact_status(mastodont_t* api)
{
    struct mstdnt_storage storage;
    if (!(post.itype && post.id)) return 1;

    // Pretty up the type
    if (strcmp(post.itype, "like") == 0)
    {
        mastodont_favourite_status(api,
                                   post.id,
                                   &storage);
        // TODO Cleanup when errors handled
        // mastodont_storage_cleanup(&storage);
    }

    return 0;
}

char* construct_status(struct mstdnt_status* status, int* size)
{
    char* stat_html;
    size_t s = easprintf(&stat_html, data_status_html,
                         status->account.avatar,
                         status->account.display_name, /* Username */
                         status->account.acct, /* Account */
                         "Public", /* visibility */
                         status->content,
                         status->reblogged ? "nobutton-active" : "",
                         status->id,
                         status->favourited ? "nobutton-active" : "",
                         status->id);
    if (size) *size = s;
    return stat_html;
}

char* construct_statuses(struct mstdnt_status* statuses, size_t size, size_t* ret_size)
{
    char* stat_html, *result = NULL;
    int curr_parse_size = 0, last_parse_size, parse_size;

    if (size <= 0) return NULL;

    for (size_t i = 0; i < size; ++i)
    {
        stat_html = construct_status(statuses + i, &parse_size);
        
        if (parse_size == -1) /* Malloc error */
        {
            if (result) free(result);
            return NULL;
        }
        last_parse_size = curr_parse_size;
        curr_parse_size += parse_size;
        
        result = realloc(result, curr_parse_size + 1);
        if (result == NULL)
        {
            perror("malloc");
            free(stat_html);
            return NULL;
        }

        /* Copy stat_html to result in correct position */
        strncpy(result + last_parse_size, stat_html, parse_size);
        /* Cleanup */
        free(stat_html);
    }
    
    result[curr_parse_size] = '\0';

    if (ret_size) *ret_size = curr_parse_size;

    return result;
}

void content_status(mastodont_t* api, char** data, size_t data_size)
{
    char* output;
    // Status context
    struct mstdnt_storage storage, status_storage;
    struct mstdnt_status* statuses_before, *statuses_after, status;
    size_t stat_before_len, stat_after_len;
    char* before_html = NULL, *stat_html = NULL, *after_html = NULL;

    mastodont_status_context(api, data[0], &storage, &statuses_before, &statuses_after,
                             &stat_before_len, &stat_after_len);
    mastodont_view_status(api, data[0], &status_storage, &status);
    before_html = construct_statuses(statuses_before, stat_before_len, NULL);
    stat_html = construct_status(&status, NULL);
    after_html = construct_statuses(statuses_after, stat_after_len, NULL);

    easprintf(&output, "%s%s%s",
              before_html != NULL ? before_html : "",
              stat_html != NULL ? stat_html : "",
              after_html != NULL ? after_html : "");
    
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
    mastodont_storage_cleanup(&storage);
}

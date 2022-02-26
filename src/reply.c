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

#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include "reply.h"
#include "easprintf.h"

// Pages
#include "../static/post.chtml"

#define ID_REPLY_SIZE 256
#define ID_RESPONSE "<input type=\"hidden\" name=\"replyid\" value=\"%s\">"

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

#define REGEX_REPLY "<a class=\"u-url mention\".*?href=\"https:\\/\\/(.*?)\\/.*?\".*?>@<span>(.*?)<\\/span>"
#define REGEX_RESULTS_LEN 9

char* reply_status(char* id, struct mstdnt_status* status)
{
    char* content = status->content;
    size_t content_len = strlen(status->content);
    char* stat_reply;
    // Regex
    pcre* re;
    int re_results[REGEX_RESULTS_LEN];
    int rc;
    const char* error;
    int erroffset;
    int url_off, url_len, name_off, name_len;
    // Replies
    size_t replies_size, replies_size_orig;
    char* replies = malloc(replies_size = strlen(status->account.acct)+2);

    // Load first reply
    replies[0] = '@';
    strcpy(replies+1, status->account.acct);
    replies[replies_size-1] = ' ';

    // Compile regex
    re = pcre_compile(REGEX_REPLY, 0, &error, &erroffset, NULL);
    if (re == NULL)
    {
        fprintf(stderr, "Couldn't parse regex at offset %d: %s\n", erroffset, error);
        free(replies);
        pcre_free(re);
    }

    for (int ind = 0;;)
    {
        rc = pcre_exec(re, NULL, content, content_len, ind, 0, re_results, REGEX_RESULTS_LEN);
        if (rc < 0)
            break;

        // Read out
        url_off = re_results[2];
        url_len = re_results[3] - url_off;
        name_off = re_results[4];
        name_len = re_results[5] - name_off;

        replies_size_orig = replies_size;
        replies_size += url_len+name_len+3;

        // Realloc string
        replies = realloc(replies, replies_size+1);

        replies[replies_size_orig] = '@';
        memcpy(replies + replies_size_orig + 1, content + name_off, name_len);
        replies[replies_size_orig+1+name_len] = '@';
        memcpy(replies + replies_size_orig + 1 + name_len + 1, content + url_off, url_len);
        replies[replies_size-1] = ' ';

        // Store to last result
        ind = re_results[5];
    }

    replies[replies_size] = '\0';
    
    stat_reply = construct_post_box(id, replies, NULL);
    if (replies) free(replies);
    pcre_free(re);
    return stat_reply;
}

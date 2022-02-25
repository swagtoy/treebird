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

#include <regex.h>
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

#define REGEX_REPLY "<a class=\"u-url mention\".*href=\"https:\\/\\/(.*)\\/.*\".*>@<span>(.*)<\\/span>"
//#define REGEX_REPLY "<a class=\"u-url mention\".*href=\"https:\\/\\/(.*)\\/.*\".*>@<span>(.*)<\\/span>"

char* reply_status(char* id, struct mstdnt_status* status)
{
    char* content = status->content;
    char* stat_reply;
    // Regex
    regex_t regex;
    regmatch_t pmatch[2];
    regoff_t off_url, len_url, off_name, len_name;
    
    // Replies
    size_t replies_size, replies_size_orig;
    char* replies = malloc(replies_size = strlen(status->account.acct)+2);

    // Load first reply
    replies[0] = '@';
    strcpy(replies+1, status->account.acct);
    replies[replies_size-1] = ' ';

    // Compile regex
    if (regcomp(&regex, REGEX_REPLY, REG_EXTENDED))
    {
        fputs("Invalid regex!", stderr);
        free(replies);
    }

    int j = 0;
    
    for (int i = 0;; ++i)
    {
        if (regexec(&regex, content + j, 2, pmatch, 0))
            break;

        off_url = pmatch[0].rm_so + j;
        len_url = pmatch[0].rm_eo - pmatch[0].rm_so;

        off_name = pmatch[1].rm_so + j;
        len_name = pmatch[1].rm_eo - pmatch[1].rm_so;

        replies_size_orig = replies_size;
        replies_size += len_url+2;

        // Realloc string
        replies = realloc(replies, replies_size+1);

        j += off_url + len_url + off_name + len_name;
        
        replies[replies_size_orig] = '@';
        memcpy(replies + replies_size_orig + 1, content + off_url, len_url);
        replies[replies_size-1] = ' ';

    }
    
    stat_reply = construct_post_box(id, replies, NULL);
    if (replies) free(replies);
    return stat_reply;
}

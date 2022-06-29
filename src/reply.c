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

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>
#include <stdlib.h>
#include <string.h>
#include "reply.h"
#include "easprintf.h"
#include "../config.h"

// Pages
#include "../static/post.ctmpl"

#define ID_REPLY_SIZE 256
#define ID_RESPONSE "<input type=\"hidden\" name=\"replyid\" value=\"%s\">"

char* construct_post_box(char* reply_id,
                         char* default_content,
                         size_t* size)
{
    char* reply_html;
    char id_reply[ID_REPLY_SIZE];
    
    // Put hidden post request
    snprintf(id_reply, ID_REPLY_SIZE, ID_RESPONSE, reply_id);

    // Construct box
    struct post_template tdata = {
        .prefix = config_url_prefix,
        .reply_input = reply_id ? id_reply : NULL,
        .content = default_content
    };
    return tmpl_gen_post(&tdata, size);
}

/* Some comments:
 *  - Misskey does not return <span>, but we still regex to make sure it's a highlight
 *  - The order of parameters in a tag can be changed (mastodon does this),
 *    so we just grep for regex href
 *  - Misskey/Mastodon adds an @ symbol in the href param, while pleroma adds /users
 */
#define REGEX_REPLY "<a .*?href=\"https?:\\/\\/(.*?)\\/(?:@|users/)?(.*?)?\".*?>@(?:<span>)?.*?(?:<\\/span>)?"

char* reply_status(struct session* ssn, char* id, struct mstdnt_status* status)
{
    char* content = status->content;
    size_t content_len = strlen(status->content);
    char* stat_reply;
    // Regex
    pcre2_code* re;
    PCRE2_SIZE* re_results;
    pcre2_match_data* re_data;
    // Regex data
    int rc;
    int error;
    PCRE2_SIZE erroffset;
    int url_off, url_len, name_off, name_len;
    // Replies
    size_t replies_size = 0, replies_size_orig;
    char* replies = NULL;
    char* instance_domain = malloc(sizeof(config_instance_url)+sizeof("https:///")+1);

    // sscanf instead of regex works here and requires less work, we just need to trim off the slash at the end
    if (sscanf(config_instance_url, "https://%s/", instance_domain) == 0)
        if (sscanf(config_instance_url, "http://%s/", instance_domain) == 0)
        {
            free(instance_domain);
            return NULL;
        }

    instance_domain[strlen(instance_domain)] = '\0';
    // Remove ports, if any. Needed for development or if
    //  the server actually supports these
    char* port_val = strchr(instance_domain, ':');
    if (port_val) *port_val = '\0';

    // Load first reply
    if (strcmp(status->account.acct, ssn->acct.acct) != 0)
    {
        replies = malloc(replies_size = strlen(status->account.acct)+2);
        replies[0] = '@';
        strcpy(replies+1, status->account.acct);
        replies[replies_size-1] = ' ';
    }

    // Compile regex
    re = pcre2_compile((PCRE2_SPTR)REGEX_REPLY, PCRE2_ZERO_TERMINATED, 0, &error, &erroffset, NULL);
    if (re == NULL)
    {
        fprintf(stderr, "Couldn't parse regex at offset %ld: %d\n", erroffset, error);
        free(replies);
        pcre2_code_free(re);
    }

    re_data = pcre2_match_data_create_from_pattern(re, NULL);

    for (int ind = 0;;)
    {
        rc = pcre2_match(re, (PCRE2_SPTR)content, content_len, ind, 0, re_data, NULL);
        if (rc < 0)
            break;

        re_results = pcre2_get_ovector_pointer(re_data);

        // Store to last result
        ind = re_results[5];

        // Read out
        url_off = re_results[2];
        url_len = re_results[3] - url_off;
        name_off = re_results[4];
        name_len = re_results[5] - name_off;

        int instance_cmp = strncmp(instance_domain, content+url_off, url_len);
        // Is this the same as us?
        // Cut off url_len by one to slice the '/' at the end
        if (instance_cmp == 0 &&
            strncmp(ssn->acct.acct, content+name_off, name_len) == 0)
            continue;
        
        replies_size_orig = replies_size;
        replies_size += (instance_cmp!=0?url_len:0)+name_len+3-(instance_cmp==0); // Bool as int :^)

        // Realloc string
        replies = realloc(replies, replies_size+1);

        replies[replies_size_orig] = '@';
        memcpy(replies + replies_size_orig + 1, content + name_off, name_len);
        if (instance_cmp != 0)
        {
            replies[replies_size_orig+1+name_len] = '@';
            memcpy(replies + replies_size_orig + 1 + name_len + 1, content + url_off, url_len);
        }
        replies[replies_size-1] = ' ';
    }

    if (replies)
        replies[replies_size-1] = '\0';

    pcre2_match_data_free(re_data);
    
    stat_reply = construct_post_box(id, replies, NULL);
    pcre2_code_free(re);
    free(replies);
    free(instance_domain);
    return stat_reply;
}

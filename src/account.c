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
#include <stdlib.h>
#include "base_page.h"
#include "error.h"
#include "../config.h"
#include "account.h"
#include "easprintf.h"
#include "status.h"
#include "http.h"

// Files
#include "../static/index.chtml"
#include "../static/account.chtml"
#include "../static/account_info.chtml"

#define FOLLOWS_YOU_HTML "<span class=\"acct-badge\">%s</span>"

char* construct_account_info(struct mstdnt_account* acct,
                             size_t* size)
{
    char* acct_info_html;
    size_t s;

    s = easprintf(&acct_info_html, data_account_info_html,
                  acct->note);

    if (size) *size = s;
    return acct_info_html;
}

char* construct_account_page(mastodont_t* api,
                             struct mstdnt_account* acct,
                             struct mstdnt_relationship* relationship,
                             struct mstdnt_status* statuses,
                             size_t statuses_len,
                             size_t* res_size)
{
    int cleanup = 0;
    int result_size;
    char* statuses_html;
    char* follows_you = NULL;
    char* info_html = NULL;
    char* result;

    // Load statuses html
    statuses_html = construct_statuses(api, statuses, statuses_len, NULL);
    if (!statuses_html)
        statuses_html = "Error in malloc!";
    else
        cleanup = 1;

    if (acct->note && strcmp(acct->note, "") != 0)
    {
        info_html = construct_account_info(acct, NULL);
    }

    if (relationship) 
        if (MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_FOLLOWED_BY))
            easprintf(&follows_you, FOLLOWS_YOU_HTML, "Follows you");
    
    result_size = easprintf(&result, data_account_html,
                            "",
                            acct->header,
                            follows_you ? follows_you : "",
                            acct->display_name,
                            acct->acct,
                            config_url_prefix,
                            acct->id,
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_NOTIFYING) ? "un" : "",
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_NOTIFYING) ? "Unsubscribe" : "Subscribe",
                            config_url_prefix,
                            acct->id,
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_BLOCKING) ? "un" : "",
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_BLOCKING) ? "Unblock" : "Block",
                            config_url_prefix,
                            acct->id,
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_MUTING) ? "un" : "",
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_MUTING) ? "Unmute" : "Mute",
                            "Statuses",
                            acct->statuses_count,
                            "Following",
                            acct->following_count,
                            "Followers",
                            acct->followers_count,
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_FOLLOWING) ? "active" : "",
                            !relationship ? "" : MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_FOLLOWING) ? "Following!" : "Follow",
                            acct->avatar,
                            info_html ? info_html : "",
                            statuses_html);
    
    if (result_size == -1)
        result = NULL;

    if (res_size) *res_size = result_size;
    if (cleanup) free(statuses_html);
    if (info_html) free(info_html);
    if (follows_you) free(follows_you);
    return result;
}

void content_account(struct session* ssn, mastodont_t* api, char** data)
{
    char* account_page;
    struct mstdnt_account acct = { 0 };
    struct mstdnt_storage storage = { 0 }, status_storage = { 0 }, relations_storage = { 0 };
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_relationship* relationships = { 0 };
    size_t status_len = 0;
    size_t relationships_len = 0;
    int lookup_type = config_experimental_lookup ? MSTDNT_LOOKUP_ACCT : MSTDNT_LOOKUP_ID;

    if (mastodont_get_account(api, lookup_type, data[0],
                              &acct, &storage, NULL) ||
        mastodont_get_account_statuses(api, acct.id, NULL,
                                       &status_storage, &statuses, &status_len))
    {
        account_page = construct_error(status_storage.error, NULL);
    }
    else {
        /* Not an error? */
        mastodont_get_relationships(api, &(acct.id), 1, &relations_storage, &relationships, &relationships_len);
            
        account_page = construct_account_page(api,
                                              &acct,
                                              relationships,
                                              statuses,
                                              status_len,
                                              NULL);
        if (!account_page)
            exit(EXIT_FAILURE);
    }
    
    struct base_page b = {        
        .category = BASE_CAT_NONE,
        .locale = L10N_EN_US,
        .content = account_page,
        .sidebar_left = NULL
    };

    /* Output */
    render_base_page(&b, ssn, api);

    /* TODO urgent: cleanup relationships */

    /* Cleanup */
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&status_storage);
    mastodont_storage_cleanup(&relations_storage);
    mstdnt_cleanup_statuses(statuses, status_len);
    mstdnt_cleanup_relationships(relationships);
    free(account_page);
}

void content_account_action(struct session* ssn, mastodont_t* api, char** data)
{
    char* referer = getenv("HTTP_REFERER");
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_relationship acct = { 0 };
    
    if (strcmp(data[1], "follow") == 0)
        mastodont_follow_account(api, data[0], &storage, &acct);
    else if (strcmp(data[1], "unfollow") == 0)
        mastodont_unfollow_account(api, data[0], &storage, &acct);
    else if (strcmp(data[1], "mute") == 0)
        mastodont_mute_account(api, data[0], &storage, &acct);
    else if (strcmp(data[1], "unmute") == 0)
        mastodont_unmute_account(api, data[0], &storage, &acct);
    else if (strcmp(data[1], "block") == 0)
        mastodont_block_account(api, data[0], &storage, &acct);
    else if (strcmp(data[1], "unblock") == 0)
        mastodont_unblock_account(api, data[0], &storage, &acct);
    else if (strcmp(data[1], "subscribe") == 0)
        mastodont_subscribe_account(api, data[0], &storage, &acct);
    else if (strcmp(data[1], "unsubscribe") == 0)
        mastodont_unsubscribe_account(api, data[0], &storage, &acct);

    mastodont_storage_cleanup(&storage);

    redirect(REDIRECT_303, referer);
}

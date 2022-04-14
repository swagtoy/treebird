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
#define MAKE_FOCUSED_IF(test_tab) (tab == test_tab ? "focused" : "")

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

static char* account_statuses_cb(struct session* ssn, mastodont_t* api, struct mstdnt_account* acct)
{
    (void)ssn;
    char* statuses_html;
    struct mstdnt_storage storage;
    struct mstdnt_status* statuses = NULL;
    size_t statuses_len = 0;
    if (mastodont_get_account_statuses(api, acct->id, NULL, &storage, &statuses, &statuses_len))
    {
        statuses_html = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        statuses_html = construct_statuses(api, statuses, statuses_len, NULL);
        if (!statuses_html)
            statuses_html = construct_error("No statuses", E_NOTICE, 1, NULL);
    }

    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_statuses(statuses, statuses_len);
    return statuses_html;
}

static void fetch_account_page(struct session* ssn,
                               mastodont_t* api,
                               char* id,
                               enum account_tab tab,
                               char* (*callback)(struct session* ssn, mastodont_t* api, struct mstdnt_account* acct))
{
    char* account_page;
    char* data;
    struct mstdnt_storage storage = { 0 },
        relations_storage = { 0 };
    struct mstdnt_account acct = { 0 };
    struct mstdnt_relationship* relationships = NULL;
    size_t relationships_len = 0;

        
    int lookup_type = config_experimental_lookup ? MSTDNT_LOOKUP_ACCT : MSTDNT_LOOKUP_ID;
    
    if (mastodont_get_account(api, lookup_type, id,
                              &acct, &storage, NULL) ||
        mastodont_get_relationships(api, &(acct.id), 1, &relations_storage, &relationships, &relationships_len))
    {
        account_page = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        data = callback(ssn, api, 
&acct);
        account_page = construct_account_page(api,
                                              &acct,
                                              relationships,
                                              tab,
                                              data,
                                              NULL);
        if (!account_page)
            account_page = construct_error("Couldn't load page", E_ERROR, 1, NULL);
        
        free(data);
    }

    struct base_page b = {   
        .category = BASE_CAT_NONE,
        .locale = L10N_EN_US,
        .content = account_page,
        .sidebar_left = NULL
    };

    render_base_page(&b, ssn, api);
        
    /* Output */
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&relations_storage);
    mstdnt_cleanup_relationships(relationships);
    free(account_page);
}

char* construct_account_page(mastodont_t* api,
                             struct mstdnt_account* acct,
                             struct mstdnt_relationship* relationship,
                             enum account_tab tab,
                             char* content,
                             size_t* res_size)
{
    int cleanup = 0;
    int result_size;
    char* follows_you = NULL;
    char* info_html = NULL;
    char* is_blocked = NULL;
    char* result;

    // Load statuses html
    if (acct->note && strcmp(acct->note, "") != 0)
    {
        info_html = construct_account_info(acct, NULL);
    }

    if (relationship)
    {
        if (MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_FOLLOWED_BY))
            easprintf(&follows_you, FOLLOWS_YOU_HTML, "Follows you");

        if (MSTDNT_FLAG_ISSET(relationship->flags, MSTDNT_RELATIONSHIP_BLOCKED_BY))
            is_blocked = construct_error("You are blocked by this user", E_NOTICE, 0, NULL);
    }
    
    result_size = easprintf(&result, data_account_html,
                            is_blocked ? is_blocked : "",
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
                            config_url_prefix,
                            acct->acct,
                            MAKE_FOCUSED_IF(ACCT_TAB_STATUSES),
                            "Statuses",
                            config_url_prefix,
                            acct->acct,
                            MAKE_FOCUSED_IF(ACCT_TAB_SCROBBLES),
                            "Scrobbles",
                            config_url_prefix,
                            acct->acct,
                            MAKE_FOCUSED_IF(ACCT_TAB_MEDIA),
                            "Media",
                            config_url_prefix,
                            acct->acct,
                            MAKE_FOCUSED_IF(ACCT_TAB_PINNED),
                            "Pinned",
                            content);
    
    if (result_size == -1)
        result = NULL;

    if (res_size) *res_size = result_size;
    if (info_html) free(info_html);
    if (follows_you) free(follows_you);
    if (is_blocked) free(is_blocked);
    return result;
}

void content_account_statuses(struct session* ssn, mastodont_t* api, char** data)
{
    fetch_account_page(ssn, api, data[0], ACCT_TAB_STATUSES, account_statuses_cb);
}

void content_account_scrobbles(struct session* ssn, mastodont_t* api, char** data)
{
    fetch_account_page(ssn, api, data[0], ACCT_TAB_SCROBBLES, account_statuses_cb);
}


void content_account_pinned(struct session* ssn, mastodont_t* api, char** data)
{
    fetch_account_page(ssn, api, data[0], ACCT_TAB_PINNED, account_statuses_cb);
}


void content_account_media(struct session* ssn, mastodont_t* api, char** data)
{
    fetch_account_page(ssn, api, data[0], ACCT_TAB_MEDIA, account_statuses_cb);
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

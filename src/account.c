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
#include "scrobble.h"
#include "string_helpers.h"

// Files
#include "../static/account.chtml"
#include "../static/account_info.chtml"
#include "../static/account_follow_btn.chtml"

#define FOLLOWS_YOU_HTML "<span class=\"acct-badge\">%s</span>"
#define MAKE_FOCUSED_IF(tab, test_tab) ((tab) == test_tab ? "focused" : "")

char* load_account_info(struct mstdnt_account* acct,
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


static char* account_scrobbles_cb(struct session* ssn, mastodont_t* api, struct mstdnt_account* acct)
{
    (void)ssn;
    char* scrobbles_html = NULL;
    struct mstdnt_storage storage;
    struct mstdnt_scrobble* scrobbles = NULL;
    size_t scrobbles_len = 0;
    struct mstdnt_get_scrobbles_args args = {
        .max_id = NULL,
        .min_id = NULL,
        .since_id = NULL,
        .offset = 0,
        .limit = 20
    };
    
    if (mastodont_get_scrobbles(api, acct->id, &args, &storage, &scrobbles, &scrobbles_len))
    {
        scrobbles_html = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        scrobbles_html = construct_scrobbles(scrobbles, scrobbles_len, NULL);
        if (!scrobbles_html)
            scrobbles_html = construct_error("No scrobbles", E_NOTICE, 1, NULL);
    }

    mastodont_storage_cleanup(&storage);
    free(scrobbles);
    return scrobbles_html;
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
    
    if (mastodont_get_account(api, lookup_type, id, &acct, &storage))
    {
        account_page = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        // Relationships may fail
        mastodont_get_relationships(api, &(acct.id), 1, &relations_storage, &relationships, &relationships_len);
        
        data = callback(ssn, api, 
                        &acct);
        account_page = load_account_page(api,
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

size_t construct_account_page(char** result, struct account_page* page, char* content)
{
    int size;
    struct mstdnt_relationship* rel = page->relationship;
    char* follow_btn = NULL, *follow_btn_text = NULL;
    char* follows_you = NULL;
    char* info_html = NULL;
    char* is_blocked = NULL;

    // Check if note is not empty
    if (page->note && strcmp(page->note, "") != 0)
    {
        info_html = load_account_info(page->account, NULL);
    }

    if (rel)
    {
        if (MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_FOLLOWED_BY))
            easprintf(&follows_you, FOLLOWS_YOU_HTML, L10N[page->locale][L10N_FOLLOWS_YOU]);

        if (MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_BLOCKED_BY))
            is_blocked = construct_error(L10N[page->locale][L10N_BLOCKED], E_NOTICE, 0, NULL);

        if (MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_REQUESTED))
            follow_btn_text = L10N[page->locale][L10N_FOLLOW_PENDING];
        else if (MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_FOLLOWING))
            follow_btn_text = L10N[page->locale][L10N_FOLLOWING];
        else
            follow_btn_text = L10N[page->locale][L10N_FOLLOW];

        easprintf(&follow_btn, data_account_follow_btn_html,
                  config_url_prefix,
                  page->id,
                  (rel && (MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_FOLLOWING) ||
                           MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_REQUESTED))
                   ? "un" : ""),
                  (rel && MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_FOLLOWING)
                   ? "active" : ""),
                  follow_btn_text);
    }

    size = easprintf(result, data_account_html,
                     STR_NULL_EMPTY(is_blocked),
                     page->header_image,
                     STR_NULL_EMPTY(follows_you),
                     page->display_name,
                     page->acct,
                     config_url_prefix,
                     page->id,
                     (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_NOTIFYING)
                      ? "un" : ""),
                     (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_NOTIFYING)
                      ? L10N[page->locale][L10N_UNSUBSCRIBE] : L10N[page->locale][L10N_SUBSCRIBE]),
                     config_url_prefix,
                     page->id,
                     (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_BLOCKING)
                      ? "un" : ""),
                     (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_BLOCKING)
                      ? L10N[page->locale][L10N_UNBLOCK] : L10N[page->locale][L10N_BLOCK]),
                     config_url_prefix,
                     page->id,
                     (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_MUTING)
                      ? "un" : ""),
                     (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_MUTING)
                      ? L10N[page->locale][L10N_UNMUTE] : L10N[page->locale][L10N_MUTE]),
                     L10N[page->locale][L10N_TAB_STATUSES],
                     page->statuses_count,
                     L10N[page->locale][L10N_TAB_FOLLOWING],
                     page->following_count,
                     L10N[page->locale][L10N_TAB_FOLLOWERS],
                     page->followers_count,
                     STR_NULL_EMPTY(follow_btn),
                     page->profile_image,
                     STR_NULL_EMPTY(info_html),
                     config_url_prefix,
                     page->acct,
                     MAKE_FOCUSED_IF(page->tab, ACCT_TAB_STATUSES),
                     L10N[page->locale][L10N_TAB_STATUSES],
                     config_url_prefix,
                     page->acct,
                     MAKE_FOCUSED_IF(page->tab, ACCT_TAB_SCROBBLES),
                     L10N[page->locale][L10N_TAB_SCROBBLES],
                     config_url_prefix,
                     page->acct,
                     MAKE_FOCUSED_IF(page->tab, ACCT_TAB_MEDIA),
                     L10N[page->locale][L10N_TAB_MEDIA],
                     config_url_prefix,
                     page->acct,
                     MAKE_FOCUSED_IF(page->tab, ACCT_TAB_PINNED),
                     L10N[page->locale][L10N_TAB_PINNED],
                     content);

    if (info_html) free(info_html);
    if (follows_you) free(follows_you);
    if (follow_btn) free(follow_btn);
    if (is_blocked) free(is_blocked);
    return size;
}

char* load_account_page(mastodont_t* api,
                        struct mstdnt_account* acct,
                        struct mstdnt_relationship* relationship,
                        enum account_tab tab,
                        char* content,
                        size_t* res_size)
{
    size_t size;
    char* result;
    struct account_page page = {
        .locale = L10N_EN_US,
        .account = acct,
        .header_image = acct->header,
        .profile_image = acct->avatar,
        .acct = acct->acct,
        .display_name = acct->display_name,
        .statuses_count = acct->statuses_count,
        .following_count = acct->following_count,
        .followers_count = acct->followers_count,
        .note = acct->note,
        .id = acct->id,
        .tab = tab,
        .relationship = relationship,
    };

    size = construct_account_page(&result, &page, content);

    if (res_size) *res_size = size;
    return result;
}



void content_account_statuses(struct session* ssn, mastodont_t* api, char** data)
{
    fetch_account_page(ssn, api, data[0], ACCT_TAB_STATUSES, account_statuses_cb);
}

void content_account_scrobbles(struct session* ssn, mastodont_t* api, char** data)
{
    fetch_account_page(ssn, api, data[0], ACCT_TAB_SCROBBLES, account_scrobbles_cb);
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

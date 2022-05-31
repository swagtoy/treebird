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
#include "base_page.h"
#include "scrobble.h"
#include "string_helpers.h"
#include "navigation.h"

// Files
#include "../static/account.ctmpl"
#include "../static/account_info.ctmpl"
#include "../static/account_follow_btn.ctmpl"
#include "../static/favourites_page.ctmpl"
#include "../static/bookmarks_page.ctmpl"
#include "../static/account_stub.ctmpl"
#include "../static/account_sidebar.ctmpl"

#define FOLLOWS_YOU_HTML "<span class=\"acct-badge\">%s</span>"

struct account_args
{
    mastodont_t* api;
    struct mstdnt_account* accts;
    uint8_t flags;
};

char* load_account_info(struct mstdnt_account* acct,
                        size_t* size)
{
    struct account_info_template data = {
        .acct_note = acct->note
    };
    return tmpl_gen_account_info(&data, size);
}

char* construct_account_sidebar(struct mstdnt_account* acct, size_t* size)
{
    struct account_sidebar_template data = {
        .avatar = acct->avatar,
        .username = acct->display_name,
        .statuses_text = L10N[L10N_EN_US][L10N_TAB_STATUSES],
        .following_text = L10N[L10N_EN_US][L10N_TAB_FOLLOWING],
        .followers_text = L10N[L10N_EN_US][L10N_TAB_FOLLOWERS],
        .statuses_count = acct->statuses_count,
        .following_count = acct->following_count,
        .followers_count = acct->followers_count
    };
    return tmpl_gen_account_sidebar(&data, size);
}

static char* account_statuses_cb(struct session* ssn,
                                 mastodont_t* api,
                                 struct mstdnt_account* acct,
                                 void* _args)

{
    (void)ssn;
    struct mstdnt_account_statuses_args* args = _args;
    char* statuses_html = NULL, *navigation_box = NULL;
    char* output;
    struct mstdnt_storage storage;
    struct mstdnt_status* statuses = NULL;
    size_t statuses_len = 0;
    char* start_id;
    
    if (mastodont_get_account_statuses(api, acct->id, args, &storage, &statuses, &statuses_len))
    {
        statuses_html = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        statuses_html = construct_statuses(ssn, api, statuses, statuses_len, NULL, NULL);
        if (!statuses_html)
            statuses_html = construct_error("No statuses", E_NOTICE, 1, NULL);
    }

    if (statuses)
    {
        // If not set, set it
        start_id = keystr(ssn->post.start_id) ? keystr(ssn->post.start_id) : statuses[0].id;
        navigation_box = construct_navigation_box(start_id,
                                                  statuses[0].id,
                                                  statuses[statuses_len-1].id,
                                                  NULL);
    }
    easprintf(&output, "%s%s",
              STR_NULL_EMPTY(statuses_html),
              STR_NULL_EMPTY(navigation_box));

    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_statuses(statuses, statuses_len);
    if (statuses_html) free(statuses_html);
    if (navigation_box) free(navigation_box);
    return output;
}


static char* account_scrobbles_cb(struct session* ssn, mastodont_t* api, struct mstdnt_account* acct, void* _args)
{
    (void)ssn;
    (void)_args;
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

void get_account_info(mastodont_t* api, struct session* ssn)
{
    if (mastodont_verify_credentials(api, &(ssn->acct), &(ssn->acct_storage)) == 0)
    {
        ssn->logged_in = 1;
    }
}

static void fetch_account_page(struct session* ssn,
                               mastodont_t* api,
                               char* id,
                               void* args,
                               enum account_tab tab,
                               char* (*callback)(struct session* ssn, mastodont_t* api, struct mstdnt_account* acct, void* args))
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
                        &acct, args);
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
    mstdnt_cleanup_account(&acct);
    mstdnt_cleanup_relationships(relationships);
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&relations_storage);
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

        struct account_follow_btn_template data = {
            .prefix = config_url_prefix,
            .active = (rel && MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_FOLLOWING)
                       ? "active" : ""),
            .follow_text = follow_btn_text,
            .unfollow = (rel && (MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_FOLLOWING) ||
                                 MSTDNT_FLAG_ISSET(rel->flags, MSTDNT_RELATIONSHIP_REQUESTED))
                         ? "un" : ""),
            .userid = page->id,
        };
        
        follow_btn = tmpl_gen_account_follow_btn(&data, NULL);
    }

    struct account_template acct_data = {
        .block_text = STR_NULL_EMPTY(is_blocked),
        .header = page->header_image,
        .display_name = page->display_name,
        .acct = page->acct,
        .prefix = config_url_prefix,
        .userid = page->id,
        .unsubscribe = (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                                 MSTDNT_RELATIONSHIP_NOTIFYING)
                        ? "un" : ""),
        .subscribe_text =  (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                                     MSTDNT_RELATIONSHIP_NOTIFYING)
                            ? L10N[page->locale][L10N_UNSUBSCRIBE] : L10N[page->locale][L10N_SUBSCRIBE]),
        .unblock = (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                             MSTDNT_RELATIONSHIP_BLOCKING)
                      ? "un" : ""),
        .block_text = (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_BLOCKING)
                      ? L10N[page->locale][L10N_UNBLOCK] : L10N[page->locale][L10N_BLOCK]),
        .unmute = (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                            MSTDNT_RELATIONSHIP_MUTING)
                   ? "un" : ""),
        .mute_text = (rel && MSTDNT_FLAG_ISSET(rel->flags,
                                               MSTDNT_RELATIONSHIP_MUTING)
                      ? L10N[page->locale][L10N_UNMUTE] : L10N[page->locale][L10N_MUTE]),
        .tab_statuses_text = L10N[page->locale][L10N_TAB_STATUSES],
        .statuses_count = page->statuses_count,
        .tab_following_text = L10N[page->locale][L10N_TAB_FOLLOWING],
        .following_count = page->following_count,
        .tab_followers_count = L10N[page->locale][L10N_TAB_FOLLOWERS],
        .followers_count = page->followers_count,
        .follow_button = follow_btn,
        .avatar = page->profile_image,
        .info = info_html,
        .tab_status_focused = MAKE_FOCUSED_IF(page->tab, ACCT_TAB_STATUSES),
        .tab_statuses_text = L10N[page->locale][L10N_TAB_STATUSES],
        .tab_scrobbles_focused = MAKE_FOCUSED_IF(page->tab, ACCT_TAB_SCROBBLES),
        .tab_scrobbles_text = L10N[page->locale][L10N_TAB_SCROBBLES],
        .tab_media_focused = MAKE_FOCUSED_IF(page->tab, ACCT_TAB_MEDIA),
        .tab_media_text = L10N[page->locale][L10N_TAB_MEDIA],
        .tab_pinned_focused = MAKE_FOCUSED_IF(page->tab, ACCT_TAB_PINNED),
        .tab_pinned_text = L10N[page->locale][L10N_TAB_PINNED],
        .acct_content = content
    };

    *result = tmpl_gen_account(&acct_data, &size);
    
    if (info_html) free(info_html);
    if (follows_you) free(follows_you);
    if (follow_btn) free(follow_btn);
    if (is_blocked) free(is_blocked);
    return size;
}

char* construct_account(mastodont_t* api,
                        struct mstdnt_account* acct,
                        uint8_t flags,
                        int* size)
{
    struct account_stub_template data = {
        .prefix = config_url_prefix,
        .acct = acct->acct,
        .avatar = acct->avatar,
        .display_name = acct->display_name
    };

    return tmpl_gen_account_stub(&data, size);
}

static char* construct_account_voidwrap(void* passed, size_t index, int* res)
{
    struct account_args* args = passed;
    return construct_account(args->api, args->accts + index, args->flags, res);
}

char* construct_accounts(mastodont_t* api,
                         struct mstdnt_account* accounts,
                         size_t size,
                         uint8_t flags,
                         size_t* ret_size)
{
    if (!(accounts && size)) return NULL;
    struct account_args acct_args = {
        .api = api,
        .accts = accounts,
        .flags = flags,
    };
    return construct_func_strings(construct_account_voidwrap, &acct_args, size, ret_size);
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
    
    struct mstdnt_account_statuses_args args = {
        .pinned = 0,
        .only_media = 0,
        .with_muted = 0,
        .exclude_reblogs = 0,
        .exclude_replies = 0,
        .tagged = NULL,
        .max_id = keystr(ssn->post.max_id),
        .min_id = keystr(ssn->post.min_id),
        .since_id = NULL,
        .offset = 0,
        .limit = 20,
    };
    fetch_account_page(ssn, api, data[0], &args, ACCT_TAB_STATUSES, account_statuses_cb);
}

void content_account_scrobbles(struct session* ssn, mastodont_t* api, char** data)
{
    fetch_account_page(ssn, api, data[0], NULL, ACCT_TAB_SCROBBLES, account_scrobbles_cb);
}


void content_account_pinned(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_account_statuses_args args = {
        .pinned = 1,
        .only_media = 0,
        .with_muted = 0,
        .exclude_reblogs = 0,
        .exclude_replies = 0,
        .tagged = NULL,
        .max_id = keystr(ssn->post.max_id),
        .min_id = keystr(ssn->post.min_id),
        .since_id = NULL,
        .offset = 0,
        .limit = 20,
    };
    
    fetch_account_page(ssn, api, data[0], &args, ACCT_TAB_PINNED, account_statuses_cb);
}


void content_account_media(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_account_statuses_args args = {
        .pinned = 0,
        .only_media = 1,
        .with_muted = 0,
        .exclude_reblogs = 0,
        .exclude_replies = 0,
        .tagged = NULL,
        .max_id = keystr(ssn->post.max_id),
        .min_id = keystr(ssn->post.min_id),
        .since_id = NULL,
        .offset = 0,
        .limit = 20,
    };
    
    fetch_account_page(ssn, api, data[0], &args, ACCT_TAB_MEDIA, account_statuses_cb);
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

void content_account_bookmarks(struct session* ssn, mastodont_t* api, char** data)
{
    size_t status_count = 0, statuses_html_count = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };
    char* status_format = NULL,
        *navigation_box = NULL,
        *output = NULL;
    char* start_id;

    struct mstdnt_bookmarks_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };

    if (mastodont_get_bookmarks(api, &args, &storage, &statuses, &status_count))
    {
        status_format = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        // Construct statuses into HTML
        status_format = construct_statuses(ssn, api, statuses, status_count, NULL, &statuses_html_count);
        if (!status_format)
            status_format = construct_error("Couldn't load posts", E_ERROR, 1, NULL);
    }

    // Create post box
    if (statuses)
    {
        // If not set, set it
        start_id = keystr(ssn->post.start_id) ? keystr(ssn->post.start_id) : statuses[0].id;
        navigation_box = construct_navigation_box(start_id,
                                                  statuses[0].id,
                                                  statuses[status_count-1].id,
                                                  NULL);
    }

    struct bookmarks_page_template data = {
        .statuses = status_format,
        .navigation = navigation_box
    };
    output = tmpl_gen_bookmarks_page(&data, NULL);

    struct base_page b = {
        .category = BASE_CAT_BOOKMARKS,
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_statuses(statuses, status_count);
    if (status_format) free(status_format);
    if (navigation_box) free(navigation_box);
    if (output) free(output);
}

void content_account_favourites(struct session* ssn, mastodont_t* api, char** data)
{
    size_t status_count = 0, statuses_html_count = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };
    char* status_format = NULL,
        *navigation_box = NULL,
        *output = NULL,
        *page = NULL;
    char* start_id;

    struct mstdnt_favourites_args args = {
        .max_id = keystr(ssn->post.max_id),
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };

    if (mastodont_get_favourites(api, &args, &storage, &statuses, &status_count))
    {
        status_format = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        // Construct statuses into HTML
        status_format = construct_statuses(ssn, api, statuses, status_count, NULL, &statuses_html_count);
        if (!status_format)
            status_format = construct_error("Couldn't load posts", E_ERROR, 1, NULL);
    }

    // Create post box
    if (statuses)
    {
        // If not set, set it
        start_id = keystr(ssn->post.start_id) ? keystr(ssn->post.start_id) : statuses[0].id;
        navigation_box = construct_navigation_box(start_id,
                                                  statuses[0].id,
                                                  statuses[status_count-1].id,
                                                  NULL);
    }

    struct favourites_page_template data = {
        .statuses = status_format,
        .navigation = navigation_box
    };
    output = tmpl_gen_favourites_page(&data, NULL);

    struct base_page b = {
        .category = BASE_CAT_FAVOURITES,
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_statuses(statuses, status_count);
    if (status_format) free(status_format);
    if (navigation_box) free(navigation_box);
    if (output) free(output);
}

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
#include "global_perl.h"
#include "helpers.h"
#include "base_page.h"
#include "error.h"
#include "../config.h"
#include "account.h"
#include "easprintf.h"
#include "status.h"
#include "http.h"
#include "string.h"
#include "base_page.h"
#include "scrobble.h"
#include "string_helpers.h"
#include "navigation.h"
#include "emoji.h"
#include "timeline.h"

// Files
#include "../static/account.ctmpl"
#include "../static/account_info.ctmpl"
#include "../static/account_follow_btn.ctmpl"
#include "../static/favourites_page.ctmpl"
#include "../static/bookmarks_page.ctmpl"
#include "../static/account_stub.ctmpl"
#include "../static/account_sidebar.ctmpl"
#include "../static/account_current_menubar.ctmpl"
#include "../static/basic_page.ctmpl"

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
    char* info_html;
    char* note = emojify(acct->note,
                         acct->emojis,
                         acct->emojis_len);
    struct account_info_template data = {
        .acct_note = note
    };
    info_html = tmpl_gen_account_info(&data, size);
    if (note != acct->note)
        free(note);
    return info_html;
}

static char* accounts_page(HV* session_hv,
                           mastodont_t* api,
                           struct mstdnt_account* acct,
                           struct mstdnt_relationship* rel,
                           char* header,
                           struct mstdnt_storage* storage,
                           struct mstdnt_account* accts,
                           size_t accts_len)
{
    char* output;

    PERL_STACK_INIT;
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (acct)
        XPUSHs(newRV_noinc((SV*)perlify_account(acct)));
    else ARG_UNDEFINED();
    if (rel)
        XPUSHs(newRV_noinc((SV*)perlify_relationship(rel)));
    else ARG_UNDEFINED();
    
    if (accts && accts_len)
        XPUSHs(newRV_noinc((SV*)perlify_accounts(accts, accts_len)));
    else ARG_UNDEFINED();

    // perlapi doesn't specify if a string length of 0 calls strlen so calling just to be safe...
    if (header)
        mXPUSHp(header, strlen(header));

    PERL_STACK_SCALAR_CALL("account::content_accounts");
    
    output = PERL_GET_STACK_EXIT;

    mastodont_storage_cleanup(storage);
    mstdnt_cleanup_accounts(accts, accts_len);
    return output;
}

char* construct_account_sidebar(struct mstdnt_account* acct, size_t* size)
{
    char* result = NULL;
    char* sanitized_display_name = NULL;
    char* display_name = NULL;
    char* header_css = NULL;
    if (acct->display_name)
    {
        sanitized_display_name = sanitize_html(acct->display_name);
        display_name = emojify(sanitized_display_name,
                               acct->emojis,
                               acct->emojis_len);
    }
    easprintf(&header_css, "style=\"background: linear-gradient(var(--account-overlay-gradient-top), var(--account-overlay-gradient-bottom)), url(%s);\"", acct->header);
    struct account_sidebar_template data = {
        .prefix = config_url_prefix,
        .avatar = acct->avatar,
        .username = display_name,
        .header = acct->header ? header_css : "",
        .statuses_text = L10N[L10N_EN_US][L10N_TAB_STATUSES],
        .following_text = L10N[L10N_EN_US][L10N_TAB_FOLLOWING],
        .followers_text = L10N[L10N_EN_US][L10N_TAB_FOLLOWERS],
        .statuses_count = acct->statuses_count,
        .following_count = acct->following_count,
        .followers_count = acct->followers_count,
        .acct = acct->acct,
    };
    result = tmpl_gen_account_sidebar(&data, size);
    if (sanitized_display_name != acct->display_name) free(sanitized_display_name);
    if (display_name != sanitized_display_name &&
        display_name != acct->display_name)
        free(display_name);
    free(header_css);
    return result;
}

static char* account_followers_cb(HV* session_hv,
                                  struct session* ssn,
                                  mastodont_t* api,
                                  struct mstdnt_account* acct,
                                  struct mstdnt_relationship* rel, 
                                  void* _args)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_account* accounts = NULL;
    size_t accts_len = 0;
    char* result;
    
    struct mstdnt_account_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = 0,
        .limit = 20,
        .with_relationships = 0,
    };
    
    mastodont_get_followers(api, &m_args, acct->id, &args, &storage, &accounts, &accts_len);

    return accounts_page(session_hv, api, acct, rel, NULL, &storage, accounts, accts_len);
}

static char* account_following_cb(HV* session_hv,
                                  struct session* ssn,
                                  mastodont_t* api,
                                  struct mstdnt_account* acct,
                                  struct mstdnt_relationship* rel, 
                                  void* _args)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_account* accounts = NULL;
    size_t accts_len = 0;
    char* result;
    
    struct mstdnt_account_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = 0,
        .limit = 20,
        .with_relationships = 0,
    };
    
    mastodont_get_following(api, &m_args, acct->id, &args, &storage, &accounts, &accts_len);

    return accounts_page(session_hv, api, acct, rel, NULL, &storage, accounts, accts_len);
}

static char* account_statuses_cb(HV* session_hv,
                                 struct session* ssn,
                                 mastodont_t* api,
                                 struct mstdnt_account* acct,
                                 struct mstdnt_relationship* rel, 
                                 void* _args)

{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_account_statuses_args* args = _args;
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_status* statuses = NULL;
    size_t statuses_len = 0;
    char* result;
    
    mastodont_get_account_statuses(api, &m_args, acct->id, args, &storage, &statuses, &statuses_len);

    PERL_STACK_INIT;
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    XPUSHs(newRV_noinc((SV*)perlify_account(acct)));
    if (rel)
        XPUSHs(newRV_noinc((SV*)perlify_relationship(rel)));
    else ARG_UNDEFINED();
    
    if (statuses && statuses_len)
        XPUSHs(newRV_noinc((SV*)perlify_statuses(statuses, statuses_len)));
    else ARG_UNDEFINED();

    PERL_STACK_SCALAR_CALL("account::content_statuses");
    
    result = PERL_GET_STACK_EXIT;
    
    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_statuses(statuses, statuses_len);
    
    return result;
}

static char* account_scrobbles_cb(HV* session_hv,
                                  struct session* ssn,
                                  mastodont_t* api,
                                  struct mstdnt_account* acct,
                                  struct mstdnt_relationship* rel, 
                                  void* _args)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_scrobble* scrobbles = NULL;
    size_t scrobbles_len = 0;
    char* result;

    struct mstdnt_get_scrobbles_args args = {
        .max_id = NULL,
        .min_id = NULL,
        .since_id = NULL,
        .offset = 0,
        .limit = 20
    };
    mastodont_get_scrobbles(api, &m_args, acct->id, &args, &storage, &scrobbles, &scrobbles_len);

    PERL_STACK_INIT;
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    XPUSHs(newRV_noinc((SV*)perlify_account(acct)));
    if (rel)
        XPUSHs(newRV_noinc((SV*)perlify_relationship(rel)));
    else ARG_UNDEFINED();
    
    if (scrobbles && scrobbles_len)
        XPUSHs(newRV_noinc((SV*)perlify_scrobbles(scrobbles, scrobbles_len)));
    else ARG_UNDEFINED();

    PERL_STACK_SCALAR_CALL("account::content_scrobbles");
    
    result = PERL_GET_STACK_EXIT;

    mastodont_storage_cleanup(&storage);
    return result;
}

void get_account_info(mastodont_t* api, struct session* ssn)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    if (ssn->cookies.access_token.is_set && mastodont_verify_credentials(api, &m_args, &(ssn->acct), &(ssn->acct_storage)) == 0)
    {
        ssn->logged_in = 1;
    }
}

/**
 * Fetches the account information, and then calls a callback on the information received which
 * passes the account information
 *
 * @param req The request context
 * @param ssn The session, which will get transcribed into Perl
 * @param api Initiated mstdnt API
 * @param id User's ID to fetch
 * @param args The arguments to pass into the callback
 * @param tab Current tab to focus
 * @param callback Calls back with a perlified session, session and api as you passed in, the account,
 *                 the relationship, and additional arguments passed
 */
static void fetch_account_page(FCGX_Request* req,
                               struct session* ssn,
                               mastodont_t* api,
                               char* id,
                               void* args,
                               enum account_tab tab,
                               char* (*callback)(HV* ssn_hv, struct session* ssn, mastodont_t* api, struct mstdnt_account* acct, struct mstdnt_relationship* rel, void* args))
{
    struct mstdnt_storage storage = { 0 },
        relations_storage = { 0 };
    struct mstdnt_account acct = { 0 };
    struct mstdnt_relationship* relationships = NULL;
    size_t relationships_len = 0;
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    int lookup_type = config_experimental_lookup ? MSTDNT_LOOKUP_ACCT : MSTDNT_LOOKUP_ID;
    
    mastodont_get_account(api, &m_args, lookup_type, id, &acct, &storage);
    // Relationships may fail
    mastodont_get_relationships(api, &m_args, &(acct.id), 1, &relations_storage, &relationships, &relationships_len);

    HV* session_hv = perlify_session(ssn);
    
    char* data = callback(session_hv, ssn, api, &acct, relationships, args);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = data,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
        
    /* Output */
    mstdnt_cleanup_account(&acct);
    mstdnt_cleanup_relationships(relationships);
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&relations_storage);
    Safefree(data);
}

size_t construct_account_page(struct session* ssn,
                              char** result,
                              struct account_page* page,
                              char* content)
{
    if (!page->account)
    {
        *result = NULL;
        return 0;
    }
    size_t size;
    struct mstdnt_relationship* rel = page->relationship;
    int is_same_user = ssn->logged_in && strcmp(ssn->acct.acct, page->acct) == 0;
    char* follow_btn = NULL,
        * follow_btn_text = NULL,
        * follows_you = NULL,
        * info_html = NULL,
        * is_blocked = NULL,
        * menubar = NULL,
        * display_name = NULL,
        * sanitized_display_name = NULL;

    sanitized_display_name = sanitize_html(page->display_name);
    display_name = emojify(sanitized_display_name,
                           page->account->emojis,
                           page->account->emojis_len);

    // Check if note is not empty
    if (page->note && strcmp(page->note, "") != 0)
    {
        info_html = load_account_info(page->account, NULL);
    }

    // Display follow button only if not the same user
    if (rel && !is_same_user)
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

    // Display menubar with extra options for access if same user
    if (is_same_user)
    {
        struct account_current_menubar_template acmdata = {
            .prefix = config_url_prefix,
            .blocked_str = "Blocks",
            .muted_str = "Mutes",
            .favourited_str = "Favorites",
        };

        menubar = tmpl_gen_account_current_menubar(&acmdata, NULL);
    }

    struct account_template acct_data = {
        .is_blocked = STR_NULL_EMPTY(is_blocked),
        .header = page->header_image,
        .menubar = menubar,
        .display_name = display_name,
        .acct = page->acct,
        .prefix = config_url_prefix,
        .userid = page->id,
        .follows_you = follows_you,
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
        .tab_followers_text = L10N[page->locale][L10N_TAB_FOLLOWERS],
        .followers_count = page->followers_count,
        .follow_btn = follow_btn,
        .avatar = page->profile_image,
        .info = info_html,
        .tab_statuses_focused = MAKE_FOCUSED_IF(page->tab, ACCT_TAB_STATUSES),
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
    
    free(info_html);
    free(follows_you);
    free(follow_btn);
    free(is_blocked);
    free(menubar);
    if (sanitized_display_name != page->display_name) free(sanitized_display_name);
    if (display_name != page->display_name &&
        display_name != sanitized_display_name)
        free(display_name);
    return size;
}

char* construct_account(mastodont_t* api,
                        struct mstdnt_account* acct,
                        uint8_t flags,
                        size_t* size)
{
    char* result;
    char* sanitized_display_name = sanitize_html(acct->display_name);
    struct account_stub_template data = {
        .prefix = config_url_prefix,
        .acct = acct->acct,
        .avatar = acct->avatar,
        .display_name = sanitized_display_name,
    };

    result = tmpl_gen_account_stub(&data, size);
    if (sanitized_display_name != acct->display_name) free(sanitized_display_name);
    return result;
}

static char* construct_account_voidwrap(void* passed, size_t index, size_t* res)
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

char* load_account_page(struct session* ssn,
                        mastodont_t* api,
                        struct mstdnt_account* acct,
                        struct mstdnt_relationship* relationship,
                        enum account_tab tab,
                        char* content,
                        size_t* res_size)
{
    size_t size;
    char* result;
    struct account_page page = {
        .locale = l10n_normalize(ssn->config.lang),
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

    size = construct_account_page(ssn, &result, &page, content);

    if (res_size) *res_size = size;
    return result;
}

void content_account_statuses(PATH_ARGS)
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
        .offset = keyint(ssn->query.offset),
        .limit = 20,
    };
    
    fetch_account_page(req, ssn, api, data[0], &args, ACCT_TAB_STATUSES, account_statuses_cb);
}

void content_account_followers(PATH_ARGS)
{
    fetch_account_page(req, ssn, api, data[0], NULL, ACCT_TAB_NONE, account_followers_cb);
}

void content_account_following(PATH_ARGS)
{
    fetch_account_page(req, ssn, api, data[0], NULL, ACCT_TAB_NONE, account_following_cb);
}

void content_account_scrobbles(PATH_ARGS)
{
    fetch_account_page(req, ssn, api, data[0], NULL, ACCT_TAB_SCROBBLES, account_scrobbles_cb);
}

void content_account_pinned(PATH_ARGS)
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
    
    fetch_account_page(req, ssn, api, data[0], &args, ACCT_TAB_PINNED, account_statuses_cb);
}

void content_account_media(PATH_ARGS)
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
    
    fetch_account_page(req, ssn, api, data[0], &args, ACCT_TAB_MEDIA, account_statuses_cb);
}

void content_account_action(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_relationship acct = { 0 };
    
    if (strcmp(data[1], "follow") == 0)
        mastodont_follow_account(api, &m_args, data[0], &storage, &acct);
    else if (strcmp(data[1], "unfollow") == 0)
        mastodont_unfollow_account(api, &m_args, data[0], &storage, &acct);
    else if (strcmp(data[1], "mute") == 0)
        mastodont_mute_account(api, &m_args, data[0], &storage, &acct);
    else if (strcmp(data[1], "unmute") == 0)
        mastodont_unmute_account(api, &m_args, data[0], &storage, &acct);
    else if (strcmp(data[1], "block") == 0)
        mastodont_block_account(api, &m_args, data[0], &storage, &acct);
    else if (strcmp(data[1], "unblock") == 0)
        mastodont_unblock_account(api, &m_args, data[0], &storage, &acct);
    else if (strcmp(data[1], "subscribe") == 0)
        mastodont_subscribe_account(api, &m_args, data[0], &storage, &acct);
    else if (strcmp(data[1], "unsubscribe") == 0)
        mastodont_unsubscribe_account(api, &m_args, data[0], &storage, &acct);

    mastodont_storage_cleanup(&storage);

    redirect(req, REDIRECT_303, referer);
}

void content_account_bookmarks(PATH_ARGS)
{
    size_t statuses_len = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_bookmarks_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    mastodont_get_bookmarks(api, &m_args, &args, &storage, &statuses, &statuses_len);

    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_BOOKMARKS, "Bookmarks", 0, 1);
}

void content_account_blocked(PATH_ARGS)
{
    struct mstdnt_account_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = 0,
        .limit = 20,
        .with_relationships = 0,
    };
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_account* accts = NULL;
    size_t accts_len = 0;
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    
    mastodont_get_blocks(api, &m_args, &args, &storage, &accts, &accts_len);

    HV* session_hv = perlify_session(ssn);
    char* result = accounts_page(session_hv, api, NULL, NULL, "Blocked users", &storage, accts, accts_len);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = result,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    free(result);
}

void content_account_muted(PATH_ARGS)
{
    struct mstdnt_account_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = 0,
        .limit = 20,
        .with_relationships = 0,
    };
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_account* accts = NULL;
    size_t accts_len = 0;
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    
    mastodont_get_mutes(api, &m_args, &args, &storage, &accts, &accts_len);

    HV* session_hv = perlify_session(ssn);
    char* result = accounts_page(session_hv, api, NULL, NULL, "Muted users", &storage, accts, accts_len);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = result,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    free(result);
}

void content_account_favourites(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    size_t statuses_len = 0;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };

    struct mstdnt_favourites_args args = {
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };

    mastodont_get_favourites(api, &m_args, &args, &storage, &statuses, &statuses_len);
    
    content_timeline(req, ssn, api, &storage, statuses, statuses_len, BASE_CAT_BOOKMARKS, "Favorites", 0, 1);
}

AV* perlify_accounts(const struct mstdnt_account* accounts, size_t len)
{
    if (!(accounts && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_account(accounts + i)));
    }

    return av;
}

HV* perlify_scrobble(const struct mstdnt_scrobble* scrobble)
{
    if (!scrobble) return NULL;
    HV* scrobble_hv = newHV();

    hvstores_ref(scrobble_hv, "account", perlify_account(&(scrobble->account)));
    hvstores_str(scrobble_hv, "album", scrobble->album);
    hvstores_str(scrobble_hv, "artist", scrobble->artist);
    hvstores_int(scrobble_hv, "created_at", scrobble->created_at);
    hvstores_str(scrobble_hv, "id", scrobble->id);
    hvstores_int(scrobble_hv, "length", scrobble->length);
    hvstores_str(scrobble_hv, "title", scrobble->title);
    
    return scrobble_hv;
}

AV* perlify_scrobbles(const struct mstdnt_scrobble* scrobbles, size_t len)
{
    if (!(scrobbles && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_scrobble(scrobbles + i)));
    }

    return av;
}

HV* perlify_account(const struct mstdnt_account* acct)
{
    if (!acct) return NULL;
    HV* acct_hv = newHV();

    hvstores_str(acct_hv, "id", acct->id);
    hvstores_str(acct_hv, "username", acct->username);
    hvstores_str(acct_hv, "acct", acct->acct);
    hvstores_str(acct_hv, "display_name", acct->display_name);
    hvstores_str(acct_hv, "note", acct->note);
    hvstores_str(acct_hv, "avatar", acct->avatar);
    hvstores_str(acct_hv, "avatar_static", acct->avatar_static);
    hvstores_str(acct_hv, "header", acct->header);
    hvstores_str(acct_hv, "header_static", acct->header_static);
    hvstores_int(acct_hv, "created_at", acct->created_at);
    hvstores_str(acct_hv, "last_status_at", acct->last_status_at);
    hvstores_str(acct_hv, "mute_expires_at", acct->mute_expires_at);
    hvstores_int(acct_hv, "statuses_count", acct->statuses_count);
    hvstores_int(acct_hv, "followers_count", acct->followers_count);
    hvstores_int(acct_hv, "following_count", acct->following_count);
    hvstores_int(acct_hv, "bot", acct->bot);
    hvstores_int(acct_hv, "suspended", acct->suspended);
    hvstores_int(acct_hv, "locked", acct->locked);
    hvstores_int(acct_hv, "discoverable", acct->discoverable);
    hvstores_ref(acct_hv, "emojis", perlify_emojis(acct->emojis, acct->emojis_len));
    
    return acct_hv;
}

HV* perlify_relationship(const struct mstdnt_relationship* rel)
{
    if (!rel) return NULL;
    HV* rel_hv = newHV();

    hvstores_str(rel_hv, "id", rel->id);
    hvstores_int(rel_hv, "following", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_FOLLOWING));
    hvstores_int(rel_hv, "requested", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_REQUESTED));
    hvstores_int(rel_hv, "endoresed", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_ENDORSED));
    hvstores_int(rel_hv, "followed_by", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_FOLLOWED_BY));
    hvstores_int(rel_hv, "muting", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_MUTING));
    hvstores_int(rel_hv, "muting_notifs", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_MUTING_NOTIFS));
    hvstores_int(rel_hv, "showing_reblogs", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_SHOWING_REBLOGS));
    hvstores_int(rel_hv, "notifying", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_NOTIFYING));
    hvstores_int(rel_hv, "blocking", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_BLOCKING));
    hvstores_int(rel_hv, "domain_blocking", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_DOMAIN_BLOCKING));
    hvstores_int(rel_hv, "blocked_by", MSTDNT_T_FLAG_ISSET(rel, MSTDNT_RELATIONSHIP_BLOCKED_BY));

    return rel_hv;
}

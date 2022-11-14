/*
 * Treebird - Lightweight frontend for Pleroma
 * 
 * Licensed under BSD 3-Clause License
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
#include "emoji.h"
#include "timeline.h"
#include "types.h"

#define FOLLOWS_YOU_HTML "<span class=\"acct-badge\">%s</span>"

typedef char* (*account_page_cb)(HV* ssn_hv,
                                 struct session* ssn,
                                 mastodont_t* api,
                                 struct mstdnt_account* acct,
                                 struct mstdnt_relationship* rel,
                                 void* args);

struct account_args
{
    mastodont_t* api;
    struct mstdnt_account* accts;
    uint8_t flags;
};

/// Used to pair accounts and relations for requests
typedef struct _acct_relations_pair
{
    mstdnt_request_cb_data* acct_data;
    mstdnt_request_cb_data* rels_data;
} acct_relations_pair;

typedef struct _acct_fetch_args
{
    acct_relations_pair pair;
    char* id;
    enum account_tab tab;
    account_page_cb callback;
    void* args;
    struct request_args* req_args;
} acct_fetch_args;

static char*
accounts_page(HV* session_hv,
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
        mXPUSHs(newRV_noinc((SV*)perlify_account(acct)));
    else ARG_UNDEFINED();
    if (rel)
        mXPUSHs(newRV_noinc((SV*)perlify_relationship(rel)));
    else ARG_UNDEFINED();
    
    if (accts && accts_len)
        mXPUSHs(newRV_noinc((SV*)perlify_accounts(accts, accts_len)));
    else ARG_UNDEFINED();

    // perlapi doesn't specify if a string length of 0 calls strlen so calling just to be safe...
    if (header)
        mXPUSHp(header, strlen(header));

    PERL_STACK_SCALAR_CALL("account::content_accounts");
    
    output = PERL_GET_STACK_EXIT;

    return output;
}

static char*
account_followers_cb(HV* session_hv,
                     struct session* ssn,
                     mastodont_t* api,
                     struct mstdnt_account* acct,
                     struct mstdnt_relationship* rel, 
                     void* _args)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* result;
    
    struct mstdnt_account_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = 0,
        .limit = 20,
        .with_relationships = 0,
    };
    
    mstdnt_get_followers(api, &m_args, NULL, NULL, acct->id, &args);

    return accounts_page(session_hv, api, acct, rel, NULL,
                         &storage, accounts, accts_len);
}

static char*
account_following_cb(HV* session_hv,
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
    
    mstdnt_get_following(api, &m_args, NULL, NULL, acct->id, &args, &storage, &accounts, &accts_len);

    return accounts_page(session_hv, api, acct, rel, NULL, &storage, accounts, accts_len);
}

static char*
account_statuses_cb(HV* session_hv,
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
    
    mstdnt_get_account_statuses(api, &m_args, NULL, NULL, acct->id, args, &storage, &statuses, &statuses_len);

    PERL_STACK_INIT;
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    mXPUSHs(newRV_noinc((SV*)perlify_account(acct)));
    if (rel)
        mXPUSHs(newRV_noinc((SV*)perlify_relationship(rel)));
    else ARG_UNDEFINED();
    
    if (statuses && statuses_len)
        mXPUSHs(newRV_noinc((SV*)perlify_statuses(statuses, statuses_len)));
    else ARG_UNDEFINED();

    PERL_STACK_SCALAR_CALL("account::content_statuses");
    
    result = PERL_GET_STACK_EXIT;
    
    return result;
}

static char*
account_scrobbles_cb(HV* session_hv,
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
    mstdnt_get_scrobbles(api, &m_args, NULL, NULL, acct->id, &args, &storage, &scrobbles, &scrobbles_len);

    PERL_STACK_INIT;
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    mXPUSHs(newRV_noinc((SV*)perlify_account(acct)));
    if (rel)
        mXPUSHs(newRV_noinc((SV*)perlify_relationship(rel)));
    else ARG_UNDEFINED();
    
    if (scrobbles && scrobbles_len)
        mXPUSHs(newRV_noinc((SV*)perlify_scrobbles(scrobbles, scrobbles_len)));
    else ARG_UNDEFINED();

    PERL_STACK_SCALAR_CALL("account::content_scrobbles");
    
    result = PERL_GET_STACK_EXIT;

    return result;
}

// TODO
void get_account_info(mastodont_t* api, struct session* ssn)
{
#if 0
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    if (ssn->cookies.access_token.is_set && mstdnt_verify_credentials(api, &m_args, NULL, NULL, &(ssn->acct), &(ssn->acct_storage)) == 0)
    {
        ssn->logged_in = 1;
    }
#endif
}

static void
generate_account_page(acct_fetch_args* args)
{
    HV* session_hv = perlify_session(args->ssn);
    
    char* data = callback(session_hv,
                          args->ssn,
                          args->api,
                          args->pair.acct,
                          args->pair.rels,
                          args);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = data,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
        
    /* Output */
    tb_free(data);
    // CLEANUP REST OF TEH DATA
    request_args_cleanup(args->req_args);
}

// Callback: fetch_account_page
static int
request_cb_account_page_relationships(mstdnt_request_cb_data* cb_data,
                                      void* tbargs)
{
    DESTRUCT_TB_ARGS(tbargs);

    acct_fetch_args* data = args;
    data->pair.rels_data = cb_data;
    // TODO if this fails (it definitely can), then set to 0x1
    if (data->pair.rels_data->data && data->pair.acct_data->data)
    {
        generate_account_page(data);
    }

    return MSTDNT_REQUEST_DATA_NOCLEANUP;
}

// Callback: fetch_account_page
static int
request_cb_account_page_acct(mstdnt_request_cb_data* cb_data,
                             void* tbargs)
{
    DESTRUCT_TB_ARGS(tbargs);

    acct_fetch_args* data = args;
    data->pair.acct_data = cb_data;
    if (data->pair.rels_data->data != 0x1 &&
        data->pair.rels_data->data &&
        data->pair.acct_data->data)
    {
        generate_account_page(data);
    }

    return MSTDNT_REQUEST_DATA_NOCLEANUP;
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
 * @return 1, don't end request
 */
static int
fetch_account_page(FCGX_Request* req,
                   struct session* ssn,
                   mastodont_t* api,
                   char* id,
                   void* args,
                   enum account_tab tab,
                   account_page_cb callback)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    int lookup_type = config_experimental_lookup ? MSTDNT_LOOKUP_ACCT : MSTDNT_LOOKUP_ID;

    /* Send 2 requests.
     *
     * These requests should come in any order, so create a pair to synchronize
     *  them
     */

    // Make empty
    acct_fetch_args* f_arg = tb_calloc(1, sizeof(acct_fetch_args));
    f_arg->id = id;
    f_arg->args = args;
    f_arg->tab = tab;
    f_arg->callback = callback;
    
    struct request_args* req_args =
        request_args_create(req, ssn, api, f_arg);
    f_arg->req_args = req_args;
        
    mstdnt_get_account(api,
                       &m_args,
                       request_cb_account_page_acct,
                       f_arg,
                       lookup_type,
                       id);
    
    // Relationships may fail
    mstdnt_get_relationships(api,
                             &m_args,
                             request_cb_account_page_relationships,
                             f_arg,
                             &(acct.id),
                             1);

    return 1;
}

int content_account_statuses(PATH_ARGS)
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
    
    return fetch_account_page(req, ssn, api, data[0], &args, ACCT_TAB_STATUSES, account_statuses_cb);
}

int content_account_followers(PATH_ARGS)
{
    return fetch_account_page(req, ssn, api, data[0], NULL, ACCT_TAB_NONE, account_followers_cb);
}

int content_account_following(PATH_ARGS)
{
    return fetch_account_page(req, ssn, api, data[0], NULL, ACCT_TAB_NONE, account_following_cb);
}

int content_account_scrobbles(PATH_ARGS)
{
    return fetch_account_page(req, ssn, api, data[0], NULL, ACCT_TAB_SCROBBLES, account_scrobbles_cb);
}

int content_account_pinned(PATH_ARGS)
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
    
    return fetch_account_page(req, ssn, api, data[0], &args, ACCT_TAB_PINNED, account_statuses_cb);
}

int content_account_media(PATH_ARGS)
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
    
    return fetch_account_page(req, ssn, api, data[0], &args, ACCT_TAB_MEDIA, account_statuses_cb);
}

static void
request_cb_content_account_action(mstdnt_request_cb_data* cb_data, void* tbargs)
{
    mstdnt_relationship* rel = MSTDNT_CB_DATA(cb_data);
    DESTRUCT_TB_ARGS(tbargs);
    
    redirect(req, REDIRECT_303, referer);
    return MSTDNT_REQUEST_DONE;
}


int
content_account_action(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    
    if (strcmp(data[1], "follow") == 0)
        return mstdnt_follow_account(api, &m_args,
                              request_cb_content_account_action, cb_args, data[0]);
    else if (strcmp(data[1], "unfollow") == 0)
        return mstdnt_unfollow_account(api, &m_args,
                                request_cb_content_account_action, cb_args, data[0]);
    else if (strcmp(data[1], "mute") == 0)
        return mstdnt_mute_account(api, &m_args,
                            request_cb_content_account_action, cb_args, data[0]);
    else if (strcmp(data[1], "unmute") == 0)
        return mstdnt_unmute_account(api, &m_args,
                              request_cb_content_account_action, cb_args, data[0]);
    else if (strcmp(data[1], "block") == 0)
        return mstdnt_block_account(api, &m_args,
                             request_cb_content_account_action, cb_args, data[0]);
    else if (strcmp(data[1], "unblock") == 0)
        return mstdnt_unblock_account(api, &m_args,
                               request_cb_content_account_action, cb_args, data[0]);
    else if (strcmp(data[1], "subscribe") == 0)
        return mstdnt_subscribe_account(api, &m_args,
                                 request_cb_content_account_action, cb_args, data[0]);
    else if (strcmp(data[1], "unsubscribe") == 0)
        return mstdnt_unsubscribe_account(api, &m_args,
                                   request_cb_content_account_action, cb_args, data[0]);
}

static int
request_cb_content_bookmarks(mstdnt_request_cb_data* cb_data, void* tbargs)
{
    struct mstdnt_statuses* statuses = MSTDNT_CB_DATA(cb_data);
    DESTRUCT_TB_ARGS(tbargs);
    
    content_timeline(req, ssn, api, &storage,
                     statuses->statuses, statuses->len,
                     BASE_CAT_BOOKMARKS, "Bookmarks", 0, 1);

    finish_free_request(req);
    return MSTDNT_REQUEST_DONE;
}

int content_account_bookmarks(PATH_ARGS)
{
    size_t statuses_len = 0;
    struct mstdnt_bookmarks_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    struct request_args* cb_args =
        request_args_create(req, ssn, api, NULL);

    return mstdnt_get_bookmarks(api,
                                &m_args,
                                request_cb_content_bookmarks,
                                cb_args,
                                args);

}

int content_account_blocked(PATH_ARGS)
{
#if 0
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
    
    mstdnt_get_blocks(api, &m_args, NULL, NULL, &args, &storage, &accts, &accts_len);

    HV* session_hv = perlify_session(ssn);
    char* result = accounts_page(session_hv, api, NULL, NULL, "Blocked users", &storage, accts, accts_len);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = result,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    tb_free(result);
#endif
}

// Callback: content_account_favourites
static int
request_cb_content_mutes(mstdnt_request_cb_data* cb_data, void* tbargs)
{
#if 0
    struct mstdnt_statuses* statuses = MSTDNT_CB_DATA(cb_data);
    DESTRUCT_TB_ARGS(tbargs);
    
    HV* session_hv = perlify_session(ssn);
    accounts_page(session_hv,
                  ssn,
                  api,
                  &storage,
                  statuses->statuses, statuses->len,
                  BASE_CAT_BOOKMARKS, "Favorites", 0, 1);

    finish_free_request(req);
    return MSTDNT_REQUEST_DONE;
#endif
}

int content_account_muted(PATH_ARGS)
{
#if 0
    struct mstdnt_account_args args = {
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = 0,
        .limit = 20,
        .with_relationships = 0,
    };
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    
    struct request_args* cb_args =
        request_args_create(req, ssn, api, NULL);
        
    mstdnt_get_mutes(api, &m_args,
                     request_cb_content_account_muted,
                     cb_args, args);
#endif
}

// Callback: content_account_favourites
static void
request_cb_content_favorites(mstdnt_request_cb_data* cb_data, void* tbargs)
{
    struct mstdnt_statuses* statuses = MSTDNT_CB_DATA(cb_data);
    DESTRUCT_TB_ARGS(tbargs);
    
    content_timeline(req, ssn, api, &storage,
                     statuses->statuses, statuses->len,
                     BASE_CAT_BOOKMARKS, "Favorites", 0, 1);

    finish_free_request(req);
}

int
content_account_favourites(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    struct mstdnt_favourites_args args = {
        .min_id = keystr(ssn->post.min_id),
        .limit = 20,
    };

    struct request_args* cb_args =
        request_args_create(req, ssn, api, NULL);

    return mstdnt_get_favourites(api,
                                 &m_args,
                                 request_cb_content_favorites,
                                 cb_args,
                                 args);
    
}

PERLIFY_MULTI(account, accounts, mstdnt_account)

HV*
perlify_account(const struct mstdnt_account* acct)
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

HV*
perlify_relationship(const struct mstdnt_relationship* rel)
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

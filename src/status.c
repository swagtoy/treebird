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

#include <stdlib.h>
#include <string.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include "helpers.h"
#include "http.h"
#include "base_page.h"
#include "applications.h"
#include "status.h"
#include "easprintf.h"
#include "query.h"
#include "cookie.h"
#include "string_helpers.h"
#include "error.h"
#include "attachments.h"
#include "emoji_reaction.h"
#include "../config.h"
#include "string.h"
#include "emoji.h"
#include "account.h"

#define ACCOUNT_INTERACTIONS_LIMIT 11
#define NUM_STR "%u"

int try_post_status(struct session* ssn, mstdnt_t* api)
{
    if (!(keystr(ssn->post.content))) return 1;
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    // Flip m_args to NOT (which is set by set_mstdnt_args)
    // This is because we want to upload files too, so it's just
    // a MIME post request
    m_args.flags ^= MSTDNT_FLAG_NO_URI_SANITIZE;

    struct mstdnt_storage storage = { 0 }, *att_storage = NULL;

    char** files = NULL;
    size_t files_len = 0;
    struct mstdnt_attachment* attachments = NULL;
    char** media_ids = NULL;
    cJSON* json_ids = NULL;
    size_t json_ids_len = 0;

    // Upload images
    if (!ssn->post.file_ids.is_set)
        try_upload_media(&att_storage, ssn, api, &attachments, &media_ids);
    else
    {
        // Parse json file ids
        json_ids = cJSON_Parse(keystr(ssn->post.file_ids));
        json_ids_len = cJSON_GetArraySize(json_ids);
        if (json_ids_len)
        {
            media_ids = safemalloc(json_ids_len * sizeof(char*));
            // TODO error
            cJSON* id;
            int i = 0;
            cJSON_ArrayForEach(id, json_ids)
            {
                media_ids[i] = id->valuestring;
                ++i;
            }
        }
    }

    // Cookie copy and read
    struct mstdnt_status_args args = {
        .content_type = "text/plain",
        .expires_in = 0,
        .in_reply_to_conversation_id = NULL,
        .in_reply_to_id = keystr(ssn->post.replyid),
        .language = NULL,
        .media_ids = media_ids,
        .media_ids_len = (!ssn->post.file_ids.is_set ? keyfile(ssn->post.files).array_size :
                          (json_ids ? json_ids_len : 0)),
        .poll = NULL,
        .preview = 0,
        .scheduled_at = NULL,
        .sensitive = 0,
        .spoiler_text = NULL,
        .status = keystr(ssn->post.content),
        .visibility = keystr(ssn->post.visibility),
    };

    // Finally, create (no error checking)
    mstdnt_create_status(api, &m_args, &args, &storage);

    mstdnt_storage_cleanup(&storage);
    
    if (att_storage)
        cleanup_media_storages(ssn, att_storage);
    
    if (json_ids)
        Safefree(media_ids);
    else
        cleanup_media_ids(ssn, media_ids);
    
    Safefree(attachments);
    if (json_ids) cJSON_Delete(json_ids);
    return 0;
}

int try_react_status(struct session* ssn, mstdnt_t* api, char* id, char* emoji)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_status status = { 0 };

    mstdnt_status_emoji_react(api, &m_args, id, emoji, &storage, &status);

    mstdnt_cleanup_status(&status);
    mstdnt_storage_cleanup(&storage);
    return 0;
}

void content_status_create(PATH_ARGS)
{
    char* referer = getenv("HTTP_REFERER");

    try_post_status(ssn, api);

    redirect(req, REDIRECT_303, referer);
}

void content_status_react(PATH_ARGS)
{
    char* referer = getenv("HTTP_REFERER");

    try_react_status(ssn, api, data[0], data[1]);

    redirect(req, REDIRECT_303, referer);
}

int try_interact_status(struct session* ssn, mstdnt_t* api, char* id)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    int res = 0;
    struct mstdnt_storage storage = { 0 };
    if (!(keystr(ssn->post.itype) && id)) return 1;

    // Pretty up the type
    if (strcmp(keystr(ssn->post.itype), "like") == 0 ||
        strcmp(keystr(ssn->post.itype), "likeboost") == 0)
        res = mstdnt_favourite_status(api, &m_args, id, &storage, NULL);
    // Not else if because possibly a like-boost
    if (strcmp(keystr(ssn->post.itype), "repeat") == 0 ||
        strcmp(keystr(ssn->post.itype), "likeboost") == 0)
        res = mstdnt_reblog_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "bookmark") == 0)
        res = mstdnt_bookmark_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "pin") == 0)
        res = mstdnt_pin_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "mute") == 0)
        res = mstdnt_mute_conversation(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "delete") == 0)
        res = mstdnt_delete_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unlike") == 0)
        res = mstdnt_unfavourite_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unrepeat") == 0)
        res = mstdnt_unreblog_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unbookmark") == 0)
        res = mstdnt_unbookmark_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unpin") == 0)
        res = mstdnt_unpin_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unmute") == 0)
        res = mstdnt_unmute_conversation(api, &m_args, id, &storage, NULL);

    mstdnt_storage_cleanup(&storage);
    return res;
}

char* get_in_reply_to(mstdnt_t* api,
                      struct session* ssn,
                      struct mstdnt_status* status,
                      size_t* size)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_account acct = { 0 };
    
    int res = mstdnt_get_account(api,
                                    &m_args,
                                    1,
                                    status->in_reply_to_account_id,
                                    &acct,
                                    &storage);

    char* html = "TODO";

//    char* html = construct_in_reply_to(status, res == 0 ? &acct : NULL, size);

    if (res == 0) mstdnt_cleanup_account(&acct);
    mstdnt_storage_cleanup(&storage);
    return html;
}

void status_interact(PATH_ARGS)
{
    char* referer = GET_ENV("HTTP_REFERER", req);
    
    try_interact_status(ssn, api, data[0]);

    redirect(req, REDIRECT_303, referer);
}

void api_status_interact(PATH_ARGS)
{
    if (try_interact_status(ssn, api, keystr(ssn->post.id)) == 0)
    {
        send_result(req, NULL, "application/json", "{\"status\":\"Success\"}", 0);
    }
    else
        send_result(req, NULL, "application/json", "{\"status\":\"Couldn't load status\"}", 0);
}

void status_view(PATH_ARGS)
{
    content_status(req, ssn, api, data, STATUS_FOCUSED);
}

void status_emoji(PATH_ARGS)
{
    content_status(req, ssn, api, data, STATUS_FOCUSED | STATUS_EMOJI_PICKER);
}

void status_reply(PATH_ARGS)
{
    content_status(req, ssn, api, data, STATUS_FOCUSED | STATUS_REPLY);
}

void status_view_reblogs(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_account* reblogs = NULL;
    struct mstdnt_storage storage = { 0 };
    size_t reblogs_len = 0;
    char* status_id = data[0];

    mstdnt_status_reblogged_by(api,
                                  &m_args,
                                  status_id,
                                  &storage,
                                  &reblogs,
                                  &reblogs_len);
    
    content_status_interactions(
        req,
        ssn,
        api,
        "Reblogs",
        reblogs,
        reblogs_len);

    mstdnt_storage_cleanup(&storage);
    mstdnt_cleanup_accounts(reblogs, reblogs_len);
}

void status_view_favourites(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_account* favourites = NULL;
    struct mstdnt_storage storage = { 0 };
    size_t favourites_len = 0;
    char* status_id = data[0];

    mstdnt_status_favourited_by(api,
                                   &m_args,
                                   status_id,
                                   &storage,
                                   &favourites,
                                   &favourites_len);
    
    content_status_interactions(
        req,
        ssn,
        api,
        "Favorites",
        favourites,
        favourites_len);

    mstdnt_storage_cleanup(&storage);
    mstdnt_cleanup_accounts(favourites, favourites_len);
}

void content_status_interactions(FCGX_Request* req,
                                 struct session* ssn,
                                 mstdnt_t* api,
                                 char* label,
                                 struct mstdnt_account* accts,
                                 size_t accts_len)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (accts)
        mXPUSHs(newRV_noinc((SV*)perlify_accounts(accts, accts_len)));
    else ARG_UNDEFINED();
    mXPUSHs(newSVpv(label, 0));

    PERL_STACK_SCALAR_CALL("account::status_interactions");

    char* dup = PERL_GET_STACK_EXIT;

    struct base_page page = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };
    render_base_page(&page, req, ssn, api);

    // Cleanup
    Safefree(dup);
}

void content_status(PATH_ARGS, uint8_t flags)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = {0}, status_storage = {0};
    struct mstdnt_status* statuses_before = NULL,
        *statuses_after = NULL,
        status = { 0 };
    char* picker = NULL;
    size_t picker_len;
    size_t stat_before_len = 0, stat_after_len = 0;
    
    try_post_status(ssn, api);
    mstdnt_get_status(api, &m_args, data[0], &status_storage, &status);
    mstdnt_get_status_context(api,
                                 &m_args,
                                 data[0],
                                 &storage,
                                 &statuses_before, &statuses_after,
                                 &stat_before_len, &stat_after_len);

    if ((flags & STATUS_EMOJI_PICKER) == STATUS_EMOJI_PICKER)
        picker = construct_emoji_picker(status.id, &picker_len);


    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    mXPUSHs(newRV_noinc((SV*)perlify_status(&status)));
    if (statuses_before)
        mXPUSHs(newRV_noinc((SV*)perlify_statuses(statuses_before, stat_before_len)));
    else
        ARG_UNDEFINED();
    
    if (statuses_after)
        mXPUSHs(newRV_noinc((SV*)perlify_statuses(statuses_after, stat_after_len)));
    else
        ARG_UNDEFINED();

    if (picker)
    {
        mXPUSHs(newSVpv(picker, picker_len));
    } else ARG_UNDEFINED();

    PERL_STACK_SCALAR_CALL("status::content_status");

    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);

    mstdnt_cleanup_statuses(statuses_before, stat_before_len);
    mstdnt_cleanup_statuses(statuses_after, stat_after_len);
    mstdnt_cleanup_status(&status);
    mstdnt_storage_cleanup(&storage);
    mstdnt_storage_cleanup(&status_storage);
    Safefree(dup);
    Safefree(picker);
}

void notice_redirect(PATH_ARGS)
{
    char* url;
    easprintf(&url, "%s/status/%s", config_url_prefix, data[0]);
    redirect(req, REDIRECT_303, url);
    Safefree(url);
}

HV* perlify_status_pleroma(const struct mstdnt_status_pleroma* pleroma)
{
    if (!pleroma) return NULL;
    
    HV* pleroma_hv = newHV();
    hvstores_int(pleroma_hv, "conversation_id", pleroma->conversation_id);
    hvstores_int(pleroma_hv, "direct_conversation_id", pleroma->direct_conversation_id);
    hvstores_int(pleroma_hv, "thread_muted", pleroma->thread_muted);
    hvstores_int(pleroma_hv, "local", pleroma->local);
    hvstores_int(pleroma_hv, "parent_visible", pleroma->parent_visible);
    hvstores_ref(pleroma_hv, "emoji_reactions",
                 perlify_emoji_reactions(pleroma->emoji_reactions, pleroma->emoji_reactions_len));
    hvstores_str(pleroma_hv, "expires_at", pleroma->expires_at);
    hvstores_str(pleroma_hv, "in_reply_to_account_acct", pleroma->in_reply_to_account_acct);

    return pleroma_hv;
}

HV* perlify_status(const struct mstdnt_status* status)
{
    if (!status) return NULL;
    
    HV* status_hv = newHV();
    hvstores_str(status_hv, "id", status->id);
    hvstores_str(status_hv, "uri", status->uri);
    hvstores_int(status_hv, "created_at", status->created_at);
    hvstores_ref(status_hv, "account", perlify_account(&(status->account)));
    hvstores_str(status_hv, "content", status->content);
    hvstores_str(status_hv, "spoiler_text", status->spoiler_text);
    hvstores_str(status_hv, "url", status->url);
    hvstores_str(status_hv, "in_reply_to_id", status->in_reply_to_id);
    hvstores_str(status_hv, "in_reply_to_account_id", status->in_reply_to_account_id);
    hvstores_str(status_hv, "language", status->language);
    hvstores_str(status_hv, "text", status->text);

    hvstores_int(status_hv, "favourited", status->favourited);
    hvstores_int(status_hv, "reblogged", status->reblogged);
    hvstores_int(status_hv, "muted", status->muted);
    hvstores_int(status_hv, "bookmarked", status->bookmarked);
    hvstores_int(status_hv, "pinned", status->pinned);

    hvstores_int(status_hv, "sensitive", status->sensitive);
    hvstores_int(status_hv, "visibility", ((int)(status->visibility)));
    hvstores_int(status_hv, "reblogs_count", status->reblogs_count);
    hvstores_int(status_hv, "favourites_count", status->favourites_count);
    hvstores_int(status_hv, "replies_count", status->replies_count);
    hvstores_ref(status_hv, "reblog", perlify_status(status->reblog));
    hvstores_ref(status_hv, "application", perlify_application(status->application));
    hvstores_ref(status_hv, "media_attachments",
                 perlify_attachments(status->media_attachments, status->media_attachments_len));
    hvstores_ref(status_hv, "emojis", perlify_emojis(status->emojis, status->emojis_len));
    hvstores_ref(status_hv, "pleroma", perlify_status_pleroma(&(status->pleroma)));

    return status_hv;
}

PERLIFY_MULTI(status, statuses, mstdnt_status)

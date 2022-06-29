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
#include "status.h"
#include "easprintf.h"
#include "query.h"
#include "cookie.h"
#include "string_helpers.h"
#include "error.h"
#include "reply.h"
#include "attachments.h"
#include "emoji_reaction.h"
#include "../config.h"
#include "type_string.h"
#include "string.h"
#include "emoji.h"
#include "account.h"

// Pages
#include "../static/status.ctmpl"
#include "../static/notification.ctmpl"
#include "../static/in_reply_to.ctmpl"
#include "../static/status_interactions_label.ctmpl"
#include "../static/status_interactions.ctmpl"
#include "../static/status_interaction_profile.ctmpl"
#include "../static/interactions_page.ctmpl"
#include "../static/likeboost.ctmpl"
#include "../static/reactions_btn.ctmpl"
#include "../static/interaction_buttons.ctmpl"
#include "../static/menu_item.ctmpl"
#include "../static/like_btn.ctmpl"
#include "../static/repeat_btn.ctmpl"
#include "../static/reply_btn.ctmpl"
#include "../static/expand_btn.ctmpl"
#include "../static/like_btn_img.ctmpl"
#include "../static/repeat_btn_img.ctmpl"
#include "../static/reply_btn_img.ctmpl"
#include "../static/expand_btn_img.ctmpl"

#define ACCOUNT_INTERACTIONS_LIMIT 11
#define NUM_STR "%u"

struct status_args
{
    mastodont_t* api;
    struct mstdnt_status* status;
    struct construct_statuses_args* args;
    struct session* ssn;
};

int try_post_status(struct session* ssn, mastodont_t* api)
{
    if (!(keystr(ssn->post.content))) return 1;
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    // Flip m_args to NOT (which is set by set_mstdnt_args)
    // This is because we want to upload files too, so it's just
    // a MIME post request
    m_args.flags ^= MSTDNT_FLAG_NO_URI_SANITIZE;

    struct mstdnt_storage storage = { 0 }, *att_storage = NULL;

    char** files;
    size_t files_len;
    struct mstdnt_attachment* attachments = NULL;
    char** media_ids = NULL;

    // Upload images
    try_upload_media(&att_storage, ssn, api, &attachments, &media_ids);

    // Cookie copy and read
    struct mstdnt_status_args args = {
        .content_type = "text/plain",
        .expires_in = 0,
        .in_reply_to_conversation_id = NULL,
        .in_reply_to_id = keystr(ssn->post.replyid),
        .language = NULL,
        .media_ids = media_ids,
        .media_ids_len = media_ids ? keyfile(ssn->post.files).array_size : 0,
        .poll = NULL,
        .preview = 0,
        .scheduled_at = NULL,
        .sensitive = 0,
        .spoiler_text = NULL,
        .status = keystr(ssn->post.content),
        .visibility = keystr(ssn->post.visibility),
    };

    
    mastodont_create_status(api, &m_args, &args, &storage);

    mastodont_storage_cleanup(&storage);
    if (att_storage)
        cleanup_media_storages(ssn, att_storage);
    cleanup_media_ids(ssn, media_ids);
    if (attachments) free(attachments);
    
    return 0;
}

int try_react_status(struct session* ssn, mastodont_t* api, char* id, char* emoji)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_status status = { 0 };

    mastodont_status_emoji_react(api, &m_args, id, emoji, &storage, &status);

    mstdnt_cleanup_status(&status);
    mastodont_storage_cleanup(&storage);
    return 0;
}

void content_status_create(struct session* ssn, mastodont_t* api, char** data)
{
    char* referer = getenv("HTTP_REFERER");

    try_post_status(ssn, api);

    redirect(REDIRECT_303, referer);
}

void content_status_react(struct session* ssn, mastodont_t* api, char** data)
{
    char* referer = getenv("HTTP_REFERER");

    try_react_status(ssn, api, data[0], data[1]);

    redirect(REDIRECT_303, referer);
}

const char* status_visibility_str(enum l10n_locale loc,
                                  enum mstdnt_visibility_type vis)
{
    switch (vis)
    {
    case MSTDNT_VISIBILITY_UNLISTED:
        return L10N[loc][L10N_VIS_UNLISTED];
    case MSTDNT_VISIBILITY_PRIVATE:
        return L10N[loc][L10N_VIS_PRIVATE];
    case MSTDNT_VISIBILITY_DIRECT:
        return L10N[loc][L10N_VIS_DIRECT];
    case MSTDNT_VISIBILITY_LOCAL:
        return L10N[loc][L10N_VIS_LOCAL];
    case MSTDNT_VISIBILITY_LIST:
        return L10N[loc][L10N_VIS_LIST];
    case MSTDNT_VISIBILITY_PUBLIC:
    default:
        return L10N[loc][L10N_VIS_PUBLIC];
    }
}

int try_interact_status(struct session* ssn, mastodont_t* api, char* id)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    int res = 0;
    struct mstdnt_storage storage = { 0 };
    if (!(keystr(ssn->post.itype) && id)) return 1;

    // Pretty up the type
    if (strcmp(keystr(ssn->post.itype), "like") == 0 ||
        strcmp(keystr(ssn->post.itype), "likeboost") == 0)
        res = mastodont_favourite_status(api, &m_args, id, &storage, NULL);
    // Not else if because possibly a like-boost
    if (strcmp(keystr(ssn->post.itype), "repeat") == 0 ||
        strcmp(keystr(ssn->post.itype), "likeboost") == 0)
        res = mastodont_reblog_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "bookmark") == 0)
        res = mastodont_bookmark_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "pin") == 0)
        res = mastodont_pin_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "mute") == 0)
        res = mastodont_mute_conversation(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "delete") == 0)
        res = mastodont_delete_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unlike") == 0)
        res = mastodont_unfavourite_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unrepeat") == 0)
        res = mastodont_unreblog_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unbookmark") == 0)
        res = mastodont_unbookmark_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unpin") == 0)
        res = mastodont_unpin_status(api, &m_args, id, &storage, NULL);
    else if (strcmp(keystr(ssn->post.itype), "unmute") == 0)
        res = mastodont_unmute_conversation(api, &m_args, id, &storage, NULL);

    mastodont_storage_cleanup(&storage);
    return res;
}

char* construct_status_interactions_label(char* status_id,
                                          int is_favourites,
                                          char* header,
                                          int val,
                                          size_t* size)
{
    struct status_interactions_label_template tdata = {
        .prefix = config_url_prefix,
        .status_id = status_id,
        .action = is_favourites ? "favourited_by" : "reblogged_by",
        .header = header,
        .value = val,
    };
    return tmpl_gen_status_interactions_label(&tdata, size);
}

char* construct_interaction_buttons(struct session* ssn,
                                    struct mstdnt_status* status,
                                    size_t* size,
                                    uint8_t flags)
{
    int use_img = ssn->config.interact_img;
    char* interaction_html;
    char* repeat_btn;
    char* like_btn;
    char* likeboost_html = NULL;
    char* reply_count = NULL;
    char* repeat_count = NULL;
    char* favourites_count = NULL;
    char* emoji_picker_html = NULL;
    char* reactions_btn_html = NULL;
    char* time_str;
    int show_nums = (flags & STATUS_NO_DOPAMEME) != STATUS_NO_DOPAMEME &&
        ssn->config.stat_dope;
    size_t s;

    // Emojo picker
    if ((flags & STATUS_EMOJI_PICKER) == STATUS_EMOJI_PICKER)
    {
        emoji_picker_html = construct_emoji_picker(status->id, NULL);
    }

    struct reactions_btn_template tdata = {
        .prefix = config_url_prefix,
        .status_id = status->id,
        .emoji_picker = emoji_picker_html
    };
    reactions_btn_html = tmpl_gen_reactions_btn(&tdata, NULL);

    if (show_nums)
    {
        if (status->replies_count)
            easprintf(&reply_count, NUM_STR, status->replies_count);
        if (status->reblogs_count)
            easprintf(&repeat_count, NUM_STR, status->reblogs_count);
        if (status->favourites_count)
            easprintf(&favourites_count, NUM_STR, status->favourites_count);
    }

    struct likeboost_template lbdata = {
        .prefix = config_url_prefix,
        .status_id = status->id,
    };
    likeboost_html = tmpl_gen_likeboost(&lbdata, NULL);
    
    time_str = reltime_to_str(status->created_at);

    // TODO cleanup?
    if (use_img)
    {
        struct repeat_btn_img_template rpbdata = { .prefix = config_url_prefix, .repeat_active = status->reblogged ? "active" : "" };
        repeat_btn = tmpl_gen_repeat_btn_img(&rpbdata, NULL);
        struct like_btn_img_template ldata = { .prefix = config_url_prefix, .favourite_active = status->favourited ? "active" : "" };
        like_btn = tmpl_gen_like_btn_img(&ldata, NULL);
    }
    else {
        struct repeat_btn_template rpbdata = { .repeat_active = status->reblogged ? "active" : "" };
        repeat_btn = tmpl_gen_repeat_btn(&rpbdata, NULL);
        struct like_btn_template ldata = { .favourite_active = status->favourited ? "active" : "" };
        like_btn = tmpl_gen_like_btn(&ldata, NULL);
    }

    struct interaction_buttons_template data = {
        // Icons
        .reply_btn = use_img ? data_reply_btn_img : data_reply_btn,
        .expand_btn = use_img ? data_expand_btn_img : data_expand_btn,
        .repeat_btn = repeat_btn,
        .like_btn = like_btn,
        // Interactions data
        .prefix = config_url_prefix,
        .status_id = status->id,
        .reply_count = reply_count,
        .unrepeat = status->reblogged ? "un" : "",
        .repeats_count = repeat_count,
        .repeat_text = "Repeat",
        .unfavourite = status->favourited ? "un" : "",
        .favourites_count = favourites_count,
        .favourites_text = "Favorite",
        .likeboost_btn = (likeboost_html &&
                          ssn->config.stat_oneclicksoftware &&
                          (flags & STATUS_NO_LIKEBOOST) != STATUS_NO_LIKEBOOST ? likeboost_html : ""),
        .reactions_btn = reactions_btn_html,
        .rel_time = time_str
    };

    interaction_html = tmpl_gen_interaction_buttons(&data, size);
    
    // Cleanup
    free(emoji_picker_html);
    free(reply_count);
    free(repeat_count);
    free(favourites_count);
    free(reactions_btn_html);
    free(likeboost_html);
    free(time_str);
    free(like_btn);
    free(repeat_btn);
    return interaction_html;
}

char* construct_status_interactions(char* status_id,
                                    int fav_count,
                                    int reblog_count,
                                    struct mstdnt_account* fav_accounts,
                                    size_t fav_accounts_len,
                                    struct mstdnt_account* reblog_accounts,
                                    size_t reblog_accounts_len,
                                    size_t* size)
{
    char* html;
    char* reblogs_label = reblog_count ?
        construct_status_interactions_label(status_id, 0, "Reblogs", reblog_count, NULL) : NULL;
    char* favourites_label = fav_count ?
        construct_status_interactions_label(status_id, 1, "Favorites", fav_count, NULL) : NULL;
    char* profiles = construct_status_interaction_profiles(reblog_accounts,
                                                           fav_accounts,
                                                           reblog_accounts_len,
                                                           fav_accounts_len,
                                                           NULL);
    struct status_interactions_template tdata = {
        .favourites_count = favourites_label,
        .reblogs_count = reblogs_label,
        .users = profiles
    };
    html = tmpl_gen_status_interactions(&tdata, size);
    if (reblogs_label) free(reblogs_label);
    if (favourites_label) free(favourites_label);
    if (profiles) free(profiles);
    return html;
}

char* construct_status_interaction_profile(struct interact_profile_args* args, size_t index, size_t* size)
{
    size_t s = 0;
    // Might change
    struct mstdnt_account* check_type = args->reblogs;
    char* profile_html = NULL;
    
    // Loop through reblogs first, then favourites
    if (index >= args->reblogs_len)
    {
        index -= args->reblogs_len;
        check_type = args->favourites;
    }

    // If favourites, loops through reblogs to verify no duplicates
    if (check_type == args->favourites)
    {
        for (size_t i = 0; i < args->reblogs_len; ++i)
            if (strcmp(check_type[index].id, args->reblogs[i].id) == 0)
            {
                if (size) *size = 0;
                return NULL;
            }
    }

    // Usually means no reblogs if check_type is NULL
    if (check_type)
    {
        struct status_interaction_profile_template tdata = {
            .acct = check_type[index].acct,
            .avatar = check_type[index].avatar
        };
        profile_html = tmpl_gen_status_interaction_profile(&tdata, &s);
    }
    
    if (size) *size = s;
    return profile_html;
}

static char* construct_status_interaction_profiles_voidwrap(void* passed, size_t index, size_t* res)
{
    struct interact_profile_args* args = passed;
    return construct_status_interaction_profile(args, index, res);
}

char* construct_status_interaction_profiles(struct mstdnt_account* reblogs,
                                            struct mstdnt_account* favourites,
                                            size_t reblogs_len,
                                            size_t favourites_len,
                                            size_t* ret_size)
{
    size_t arr_size = reblogs_len + favourites_len;
    // Set a limit to interactions
    if (arr_size > ACCOUNT_INTERACTIONS_LIMIT)
        arr_size = ACCOUNT_INTERACTIONS_LIMIT;
    
    struct interact_profile_args args = {
        .reblogs = reblogs,
        .reblogs_len = reblogs_len,
        .favourites = favourites,
        .favourites_len = favourites_len
    };
    
    return construct_func_strings(construct_status_interaction_profiles_voidwrap, &args, arr_size, ret_size);
}

char* get_in_reply_to(mastodont_t* api,
                      struct session* ssn,
                      struct mstdnt_status* status,
                      size_t* size)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_account acct = { 0 };
    
    int res = mastodont_get_account(api,
                                    &m_args,
                                    1,
                                    status->in_reply_to_account_id,
                                    &acct,
                                    &storage);

    char* html = construct_in_reply_to(status, res == 0 ? &acct : NULL, size);

    if (res == 0) mstdnt_cleanup_account(&acct);
    mastodont_storage_cleanup(&storage);
    return html;
}

char* construct_in_reply_to(struct mstdnt_status* status,
                            struct mstdnt_account* account,
                            size_t* size)
{
    struct in_reply_to_template tdata = {
        .prefix = config_url_prefix,
        .in_reply_to_text = L10N[L10N_EN_US][L10N_IN_REPLY_TO],
        .acct = account ? account->acct : status->in_reply_to_id,
        .status_id = status->in_reply_to_id
    };

    return tmpl_gen_in_reply_to(&tdata, size);
}

#define REGEX_GREENTEXT "((?:^|<br/?>|\\s)&gt;.*?)(?:<br/?>|$)"

char* reformat_status(struct session* ssn,
                      char* content,
                      struct mstdnt_emoji* emos,
                      size_t emos_len)
{
    if (!content) return NULL;
    char* res = make_mentions_local(content);
    char* gt_res, *emo_res;
    
    if (emos)
    {
        emo_res = emojify(res, emos, emos_len);
        if (emo_res != res && res != content)
            free(res);
        res = emo_res;
    }
    
    if (ssn->config.stat_greentexts)
    {
        gt_res = greentextify(res);
        if (gt_res != res && res != content)
            free(res);
        res = gt_res;
    }
    
    return res;
}

#define REGEX_MENTION "<a .*?mention.*?href=\"https?:\\/\\/(.*?)\\/(?:@|users/)?(.*?)?\".*?>"

char* make_mentions_local(char* content)
{
    char* url_format;
    int error;
    PCRE2_SIZE erroffset;
    int substitute_success = 0;
    // Initial size, will be increased by 30% if pcre2_substitute cannot fit into the size
    // ...why can't pcre2 just allocate a string with the size for us? Thanks...
    size_t res_len = 1024;
    char* res = malloc(res_len);
    pcre2_code* re = pcre2_compile((PCRE2_SPTR)REGEX_MENTION,
                                   PCRE2_ZERO_TERMINATED, 0,
                                   &error, &erroffset, NULL);
    if (re == NULL)
    {
        fprintf(stderr, "Couldn't parse regex at offset %ld: %d\n", erroffset, error);
        pcre2_code_free(re);
        return NULL;
    }

    int len = easprintf(&url_format,
                        "<a target=\"_parent\" class=\"mention\" href=\"http%s://%s/@$2@$1\">",
                        config_host_url_insecure ? "" : "s",
                        getenv("HTTP_HOST"));

    int rc = -1;
    PCRE2_SIZE res_len_str;
    while (rc < 0)
    {
        res_len_str = res_len;
        rc = pcre2_substitute(
            re,
            (PCRE2_SPTR)content,
            PCRE2_ZERO_TERMINATED,
            0,
            PCRE2_SUBSTITUTE_EXTENDED | PCRE2_SUBSTITUTE_GLOBAL,
            NULL,
            NULL,
            (PCRE2_SPTR)url_format,
            len,
            (PCRE2_UCHAR*)res,
            &res_len_str
            );
        if (rc < 0)
        {
            switch (rc)
            {
            case PCRE2_ERROR_NOMEMORY:
                // Increase by 30% and try again
                res_len = (float)res_len + ((float)res_len * .3);
                res = realloc(res, res_len);
                break;
            default:
                goto out;
            }
        }
        else
            substitute_success = 1;
    }

out:
    if (!substitute_success)
        free(res);
    free(url_format);
    pcre2_code_free(re);
    return substitute_success ? res : content;
}

char* greentextify(char* content)
{
    if (!content) return NULL;
    
    int error;
    PCRE2_SIZE erroffset;
    int rc;
    int gt_off;
    int gt_len;
    char* res = content;

    // Malloc'd strings
    char* reg_string;
    char* gt_string;
    
    char* oldres = NULL;
    PCRE2_SIZE* re_results;
    pcre2_code* re = pcre2_compile((PCRE2_SPTR)REGEX_GREENTEXT, PCRE2_ZERO_TERMINATED, 0, &error, &erroffset, NULL);
    pcre2_match_data* re_data;
    if (re == NULL)
    {
        fprintf(stderr, "Couldn't parse regex at offset %ld: %d\n", erroffset, error);
        pcre2_code_free(re);
        return res;
    }

    re_data = pcre2_match_data_create_from_pattern(re, NULL);

    for (int ind = 0;;)
    {
        rc = pcre2_match(re, (PCRE2_SPTR)res, strlen(res), ind, 0, re_data, NULL);
        if (rc < 0)
            break;

        re_results = pcre2_get_ovector_pointer(re_data);

        // Store to last result
        gt_off = re_results[2];
        gt_len = re_results[3] - gt_off;

        oldres = res;

        // Malloc find/repl strings
        reg_string = malloc(gt_len + 1);
        strncpy(reg_string, res + gt_off, gt_len);
        reg_string[gt_len] = '\0';
        easprintf(&gt_string, "<span class=\"greentext\">%s</span>", reg_string);
        
        res = strrepl(res, reg_string, gt_string, STRREPL_ALL );
        if (oldres != content) free(oldres);
        ind = re_results[2] + strlen(gt_string);
        free(reg_string);
        free(gt_string);
    }

    pcre2_match_data_free(re_data);
    pcre2_code_free(re);
    return res;
}

char* construct_status(struct session* ssn,
                       mastodont_t* api,
                       struct mstdnt_status* local_status,
                       size_t* size,
                       struct mstdnt_notification* local_notif,
                       struct construct_statuses_args* args,
                       uint8_t flags)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* stat_html;

    // Counts
    char* formatted_display_name = NULL;
    char* attachments = NULL;
    char* emoji_reactions = NULL;
    char* interaction_btns = NULL;
    char* notif_info = NULL;
    char* in_reply_to_str = NULL;
    char* delete_status = NULL;
    char* pin_status = NULL;
    char* interactions_html = NULL;
    enum l10n_locale locale = l10n_normalize(ssn->config.lang);
    struct mstdnt_status* status = local_status;
    // Create a "fake" notification header which contains information for
    // the reblogged status
    struct mstdnt_notification notif_reblog;
    struct mstdnt_notification* notif = local_notif;
    struct mstdnt_account* favourites = NULL;
    struct mstdnt_account* reblogs = NULL;
    struct mstdnt_storage favourites_storage = { 0 };
    struct mstdnt_storage reblogs_storage = { 0 };
    size_t favourites_len = 0;
    size_t reblogs_len = 0;

    if (!status) return NULL;

    // If focused, show status interactions
    if ((flags & STATUS_FOCUSED) == STATUS_FOCUSED &&
        (status->reblogs_count || status->favourites_count))
    {
        if (status->favourites_count)
            mastodont_status_favourited_by(api,
                                           &m_args,
                                           status->id,
                                           &favourites_storage,
                                           &favourites,
                                           &favourites_len);
        if (status->reblogs_count)
            mastodont_status_reblogged_by(api,
                                          &m_args,
                                          status->id,
                                          &reblogs_storage,
                                          &reblogs,
                                          &reblogs_len);
        interactions_html = construct_status_interactions(status->id,
                                                          status->favourites_count,
                                                          status->reblogs_count,
                                                          favourites,
                                                          favourites_len,
                                                          reblogs,
                                                          reblogs_len,
                                                          NULL);
        mastodont_storage_cleanup(&reblogs_storage);
        mastodont_storage_cleanup(&favourites_storage);
    }

    // Repoint value if it's a reblog
    if (status->reblog)
    {
        status = status->reblog;
        // Point to our account
        notif_reblog.account = &(local_status->account);
        notif_reblog.type = MSTDNT_NOTIFICATION_REBLOG;
        notif = &notif_reblog;
    }

    // Format username with emojis
    formatted_display_name = emojify(status->account.display_name,
                                     status->account.emojis,
                                     status->account.emojis_len);

    // Format status
    char* parse_content = reformat_status(ssn, status->content, status->emojis, status->emojis_len);

    interaction_btns = construct_interaction_buttons(ssn, status, NULL, flags);
    
    // Find and replace
    if (args && args->highlight_word && parse_content != status->content)
    {
        char* parse_content_tmp;
        char* repl_str = NULL;
        easprintf(&repl_str, "<span class=\"search-highlight\">%s</span>", args->highlight_word);
        parse_content_tmp = parse_content;
        parse_content = strrepl(parse_content, args->highlight_word, repl_str, STRREPL_ALL);
        // Check if the old parse_content needed to be free'd
        if (parse_content_tmp != status->content &&
            parse_content != parse_content_tmp)
            free(parse_content_tmp);
        else // No results, move back
            parse_content = parse_content_tmp;

        free(repl_str);
    }

    // Delete status menu item and pinned, logged in only
    if (ssn->logged_in && strcmp(status->account.acct, ssn->acct.acct) == 0)
    {
        struct menu_item_template mdata = {
            .prefix = config_url_prefix,
            .status_id = status->id,
            .itype = "delete",
            .text = "Delete status"
        };
        delete_status = tmpl_gen_menu_item(&mdata, NULL);
        
        mdata.itype = status->pinned ? "unpin" : "pin";
        mdata.text = status->pinned ? "Unpin status" : "Pin status";
        pin_status = tmpl_gen_menu_item(&mdata, NULL);
    }
    
    if (status->media_attachments_len)
        attachments = construct_attachments(ssn, status->sensitive, status->media_attachments, status->media_attachments_len, NULL);
    if (status->pleroma.emoji_reactions_len)
        emoji_reactions = construct_emoji_reactions(status->id, status->pleroma.emoji_reactions, status->pleroma.emoji_reactions_len, NULL);
    if (notif && notif->type != MSTDNT_NOTIFICATION_MENTION)
    {
        char* notif_display_name = emojify(notif->account->display_name,
                                           notif->account->emojis,
                                           notif->account->emojis_len);
        struct notification_template tdata = {
            .avatar = notif->account->avatar,
            .username = notif_display_name,
            .action = (local_status->reblog ? notification_type_compact_str(notif->type) : notification_type_str(notif->type)),
            .action_item = notification_type_svg(notif->type),
        };
        notif_info = tmpl_gen_notification(&tdata, NULL);
        if (notif_display_name != notif->account->display_name)
            free(notif_display_name);
    }

    if (status->in_reply_to_id && status->in_reply_to_account_id)
        in_reply_to_str = get_in_reply_to(api, ssn, status, NULL);

    struct status_template tmpl = {
        .status_id = status->id,
        .notif_info = notif_info,
        .thread_hidden = status->muted ? "checked" : "",
        // TODO doesn't even need to be a hashtag, this is a temporary hack
        .is_cat = status->account.note && strstr(status->account.note, "isCat") ? "is-cat" : NULL,
        .is_bun = status->account.note && strstr(status->account.note, "isBun") ? " is-bun" : NULL,
        .avatar = status->account.avatar,
        .username = formatted_display_name,
        .prefix = config_url_prefix,
        .acct = status->account.acct,
        .visibility = status_visibility_str(locale, status->visibility),
        .unmute = status->muted ? "un" : "",
        .unmute_btn = status->muted ? "Unmute thread" : "Mute thread",
        .unbookmark =  status->bookmarked ? "un" : "",
        .unbookmark_btn = status->bookmarked ? "Remove Bookmark" : "Bookmark",
        .delete_status = delete_status,
        .pin_status = pin_status,
        .in_reply_to_str = in_reply_to_str,
        .status_content = parse_content,
        .attachments = attachments,
        .interactions = interactions_html,
        .emoji_reactions = emoji_reactions,
        .interaction_btns = interaction_btns
    };

    stat_html = tmpl_gen_status(&tmpl, size);
    
    // Cleanup
    if (formatted_display_name != status->account.display_name &&
        formatted_display_name) free(formatted_display_name);
    free(interaction_btns);
    free(in_reply_to_str);
    free(attachments);
    free(emoji_reactions);
    if (notif) free(notif_info);
    free(delete_status);
    free(interactions_html);
    if (parse_content != status->content)
        free(parse_content);
    return stat_html;
}

static char* construct_status_voidwrap(void* passed, size_t index, size_t* res)
{
    struct status_args* args = passed;
    return construct_status(args->ssn, args->api, args->status + index, res, NULL, args->args, 0);
}

char* construct_statuses(struct session* ssn,
                         mastodont_t* api,
                         struct mstdnt_status* statuses,
                         size_t size,
                         struct construct_statuses_args* args,
                         size_t* ret_size)
{
    if (!(statuses && size)) return NULL;
    struct status_args stat_args = {
        .api = api,
        .status = statuses,
        .args = args,
        .ssn = ssn,
    };
    return construct_func_strings(construct_status_voidwrap, &stat_args, size, ret_size);
}

void status_interact(struct session* ssn, mastodont_t* api, char** data)
{
    char* referer = getenv("HTTP_REFERER");
    
    try_interact_status(ssn, api, data[0]);
    
    printf("Status: 303 See Other\r\n"
           "Location: %s#id-%s\r\n"
           "Content-Length: 14\r\n\r\n"
           "Redirecting...",
           referer ? referer : "/",
           data[0]);
}

void api_status_interact(struct session* ssn, mastodont_t* api, char** data)
{
    if (try_interact_status(ssn, api, keystr(ssn->post.id)) == 0)
    {
        send_result(NULL, "application/json", "{\"status\":\"Success\"}", 0);
    }
    else
        send_result(NULL, "application/json", "{\"status\":\"Couldn't load status\"}", 0);
}

void status_view(struct session* ssn, mastodont_t* api, char** data)
{
    content_status(ssn, api, data, STATUS_FOCUSED);
}

void status_emoji(struct session* ssn, mastodont_t* api, char** data)
{
    content_status(ssn, api, data, STATUS_FOCUSED | STATUS_EMOJI_PICKER);
}

void status_reply(struct session* ssn, mastodont_t* api, char** data)
{
    content_status(ssn, api, data, STATUS_FOCUSED | STATUS_REPLY);
}

void status_view_reblogs(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_account* favourites = NULL;
    struct mstdnt_storage storage = { 0 };
    size_t favourites_len = 0;
    char* status_id = data[0];

    mastodont_status_reblogged_by(api,
                                  &m_args,
                                  status_id,
                                  &storage,
                                  &favourites,
                                  &favourites_len);
    
    content_status_interactions(
        ssn,
        api,
        "Reblogs",
        favourites,
        favourites_len);

    mastodont_storage_cleanup(&storage);
}

void status_view_favourites(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_account* favourites = NULL;
    struct mstdnt_storage storage = { 0 };
    size_t favourites_len = 0;
    char* status_id = data[0];

    mastodont_status_favourited_by(api,
                                   &m_args,
                                   status_id,
                                   &storage,
                                   &favourites,
                                   &favourites_len);
    
    content_status_interactions(
        ssn,
        api,
        "Favorites",
        favourites,
        favourites_len);

    mastodont_storage_cleanup(&storage);
}

void content_status_interactions(struct session* ssn,
                                 mastodont_t* api,
                                 char* label,
                                 struct mstdnt_account* accts,
                                 size_t accts_len)
{
    char* accounts_html = construct_accounts(api, accts, accts_len, 0, NULL);
    if (!accounts_html)
        accounts_html = construct_error("No accounts", E_NOTICE, 1, NULL);

    struct interactions_page_template tmpl = {
        .back_ref = getenv("HTTP_REFERER"),
        .interaction_str = label,
        .accts = accounts_html
    };

    char* output = tmpl_gen_interactions_page(&tmpl, NULL);

    struct base_page page = {
        .category = BASE_CAT_NONE,
        .content = output,
        .sidebar_left = NULL
    };
    render_base_page(&page, ssn, api);

    // Cleanup
    free(accounts_html);
    free(output);
}

void content_status(struct session* ssn, mastodont_t* api, char** data, uint8_t flags)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* output;
    // Status context
    struct mstdnt_storage storage = {0}, status_storage = {0};
    struct mstdnt_status* statuses_before = NULL,
        *statuses_after = NULL,
        status = { 0 };
    size_t stat_before_len = 0, stat_after_len = 0;
    char* before_html = NULL, *stat_html = NULL, *after_html = NULL, *stat_reply = NULL;

    try_post_status(ssn, api);
    mastodont_get_status_context(api,
                                 &m_args,
                                 data[0],
                                 &storage,
                                 &statuses_before, &statuses_after,
                                 &stat_before_len, &stat_after_len);
    
    // Get information
    if (mastodont_get_status(api, &m_args, data[0], &status_storage, &status))
    {
        stat_html = construct_error("Status not found", E_ERROR, 1, NULL);
    }
    else {
        before_html = construct_statuses(ssn, api, statuses_before, stat_before_len, NULL, 0);

        // Current status
        stat_html = construct_status(ssn, api, &status, NULL, NULL, NULL, flags);
        if ((flags & STATUS_REPLY) == STATUS_REPLY)
        {
            stat_reply = reply_status(ssn,
                                      data[0],
                                      &status);
        }
    }

    // After...
    after_html = construct_statuses(ssn, api, statuses_after, stat_after_len, NULL, 0);

    easprintf(&output, "%s%s%s%s",
              before_html ? before_html : "",
              stat_html ? stat_html : "",
              stat_reply ? stat_reply : "",
              after_html ? after_html : "");
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = output,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    free(before_html);
    free(stat_html);
    free(after_html);
    free(output);
    if ((flags & STATUS_REPLY) == STATUS_REPLY)
        free(stat_reply);
    mstdnt_cleanup_statuses(statuses_before, stat_before_len);
    mstdnt_cleanup_statuses(statuses_after, stat_after_len);
    mstdnt_cleanup_status(&status);
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&status_storage);
}

void notice_redirect(struct session* ssn, mastodont_t* api, char** data)
{
    char* url;
    easprintf(&url, "%s/status/%s", config_url_prefix, data[0]);
    redirect(REDIRECT_303, url);
    free(url);
}

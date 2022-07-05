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
#include "../config.h"
#include "conversations.h"
#include "helpers.h"
#include "string_helpers.h"
#include "error.h"
#include "base_page.h"

// Files
#include "../static/chat.ctmpl"
#include "../static/chats_page.ctmpl"
#include "../static/message.ctmpl"

struct construct_message_args
{
    struct mstdnt_message* msg;
    struct mstdnt_account* you;
    struct mstdnt_account* them;
};

char* construct_chat(struct mstdnt_chat* chat, size_t* size)
{
    char* result;
    struct chat_template data = {
        .id = chat->id,
        .prefix = config_url_prefix,
        .acct = chat->account.acct,
        .avatar = chat->account.avatar,
        .display_name = chat->account.display_name,
        .last_message = "",
    };
    result = tmpl_gen_chat(&data, size);
    return result;
}

static char* construct_chat_voidwrap(void* passed, size_t index, size_t* res)
{
    return construct_chat((struct mstdnt_chat*)passed + index, res);
}

char* construct_chats(struct mstdnt_chat* chats, size_t size, size_t* ret_size)
{
    return construct_func_strings(construct_chat_voidwrap, chats, size, ret_size);
}

char* construct_message(struct mstdnt_message* msg,
                        struct mstdnt_account* you,
                        struct mstdnt_account* them,
                        size_t* size)
{
    char* result;
    struct message_template data = {
        .id = msg->id,
        .content = msg->content
    };
    result = tmpl_gen_message(&data, size);
    return result;
}

static char* construct_message_voidwrap(void* passed, size_t index, size_t* res)
{
    struct construct_message_args* args = passed;
    return construct_message(args->msg + index, args->you, args->them, res);
}

char* construct_messages(struct mstdnt_message* messages,
                         struct mstdnt_account* you,
                         struct mstdnt_account* them,
                         size_t size,
                         size_t* ret_size)
{
    struct construct_message_args args = {
        .msg = messages,
        .you = you,
        .them = them,
    };
    return construct_func_strings(construct_message_voidwrap, &args, size, ret_size);
}

char* construct_chats_view(char* lists_string, size_t* size)
{
    struct chats_page_template data = {
        .content = lists_string,
    };
    return tmpl_gen_chats_page(&data, size);
}

void content_chats(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_chat* chats = NULL;
    size_t chats_len = 0;
    struct mstdnt_storage storage = { 0 };
    char* chats_page = NULL;
    char* chats_html = NULL;

    struct mstdnt_chats_args args = {
        .with_muted = MSTDNT_TRUE,
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = keyint(ssn->query.offset),
        .limit = 20,
    };

    if (mastodont_get_chats_v2(api, &m_args, &args, &storage, &chats, &chats_len))
    {
        chats_page = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        chats_html = construct_chats(chats, chats_len, NULL);
        if (!chats_html)
            chats_html = construct_error("No chats", E_NOTICE, 1, NULL);
        chats_page = construct_chats_view(chats_html, NULL);
    }

    struct base_page b = {
        .category = BASE_CAT_CHATS,
        .content = chats_page,
        .sidebar_left = NULL
    };
    
    // Outpuot
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    free(chats_page);
    free(chats_html);
    // TOOD cleanup chats
}

void content_chat_view(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_message* messages = NULL;
    /* struct mstdnt_account acct; */
    size_t messages_len = 0;
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_storage acct_storage = { 0 };
    // char* chats_page = NULL;
    char* messages_html = NULL;

    struct mstdnt_chats_args args = {
        .with_muted = MSTDNT_TRUE,
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = keyint(ssn->query.offset),
        .limit = 20,
    };


    if (mastodont_get_chat_messages(api, &m_args, data[0],
                                    &args, &storage, &messages, &messages_len))
    {
        messages_html = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        // Get other account
        /* if (messages_len) */
        /*     mastodont_get_account(api, &m_args, 1, messages[0].account_id */
        messages_html = construct_messages(messages, &(ssn->acct), NULL, messages_len, NULL);
        if (!messages_html)
            messages_html = construct_error("This is the start of something new...", E_NOTICE, 1, NULL);
        /* messages_html = construct_chats_view(chats_html, NULL); */
    }

    struct base_page b = {
        .category = BASE_CAT_CHATS,
        .content = messages_html,
        .sidebar_left = NULL
    };
    
    // Outpuot
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    //free(chats_page);
    free(messages_html);
    // TOOD cleanup chats
}


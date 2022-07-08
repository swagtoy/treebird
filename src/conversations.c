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
#include "../static/chat_view.ctmpl"
#include "../static/embed.ctmpl"

struct construct_message_args
{
    struct mstdnt_message* msg;
    struct mstdnt_account* you;
    struct mstdnt_account* them;
    size_t msg_size; // Read messages backwards
};

struct construct_chats_args
{
    mastodont_t* api;
    struct mstdnt_args* args;
    struct mstdnt_chat* chats;
};

char* construct_chat(mastodont_t* api,
                     struct mstdnt_args* m_args,
                     struct mstdnt_chat* chat,
                     size_t* size)
{
    char* result;
    char* msg_id = NULL;
    char* last_message = "<span class=\"empty-chat-text\">Chat created</span>";

    // Get latest message
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_message* messages = NULL;
    size_t messages_len = 0;
    
    struct mstdnt_chats_args args = {
        .with_muted = MSTDNT_TRUE,
        .offset = 0,
        .limit = 1,
    };

    if (mastodont_get_chat_messages(api, m_args, chat->id, &args, &storage,
                                    &messages, &messages_len) == 0 && messages_len == 1)
    {
        last_message = messages[0].content;
        msg_id = messages[0].id;
    }
    
    struct chat_template data = {
        .id = chat->id,
        .prefix = config_url_prefix,
        .acct = chat->account.acct,
        .avatar = chat->account.avatar,
        .display_name = chat->account.display_name,
        .message_id = msg_id,
        .last_message = last_message,
    };
    result = tmpl_gen_chat(&data, size);
    mastodont_storage_cleanup(&storage);
    // TODO cleanup messages
    return result;
}

static char* construct_chat_voidwrap(void* passed, size_t index, size_t* res)
{
    struct construct_chats_args* args = passed;
    return construct_chat(args->api, args->args, args->chats + index, res);
}

char* construct_chats(mastodont_t* api,
                      struct mstdnt_args* m_args,
                      struct mstdnt_chat* chats,
                      size_t size,
                      size_t* ret_size)
{
    struct construct_chats_args args = {
        .api = api,
        .args = m_args,
        .chats = chats,
    };
            
    return construct_func_strings(construct_chat_voidwrap, &args, size, ret_size);
}

char* construct_message(struct mstdnt_message* msg,
                        struct mstdnt_account* you,
                        struct mstdnt_account* them,
                        size_t* size)
{
    char* result;
    if (!(you && them)) return NULL;
    int is_you = strcmp(you->id, msg->account_id) == 0;
    struct message_template data = {
        .id = msg->id,
        .content = msg->content,
        .is_you = is_you ? "message-you" : NULL,
        .avatar = is_you ? you->avatar : them->avatar
    };
    result = tmpl_gen_message(&data, size);
    return result;
}

static char* construct_message_voidwrap(void* passed, size_t index, size_t* res)
{
    struct construct_message_args* args = passed;
    return construct_message(args->msg + (args->msg_size - index - 1), args->you, args->them, res);
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
        .msg_size = size
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
        chats_html = construct_chats(api, &m_args, chats, chats_len, NULL);
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

char* construct_chat_view(struct session* ssn, mastodont_t* api, char* id, size_t* len)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_message* messages = NULL;
    /* struct mstdnt_account acct; */
    size_t messages_len = 0;
    struct mstdnt_storage storage = { 0 }, storage_chat = { 0 };
    struct mstdnt_chat chat;
    struct mstdnt_storage acct_storage = { 0 };
    char* chats_page = NULL;
    char* messages_html = NULL;

    struct mstdnt_chats_args args = {
        .with_muted = MSTDNT_TRUE,
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = keyint(ssn->query.offset),
        .limit = 20,
    };
    
    if (len) *len = 0;

    if (mastodont_get_chat_messages(api, &m_args, id,
                                    &args, &storage, &messages, &messages_len) ||
        mastodont_get_chat(api, &m_args, id,
                           &storage_chat, &chat))
    {
        chats_page = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        messages_html = construct_messages(messages, &(ssn->acct), &(chat.account), messages_len, NULL);
        if (!messages_html)
            messages_html = construct_error("This is the start of something new...", E_NOTICE, 1, NULL);

        struct chat_view_template tmpl = {
            .back_link = "/chats",
            .prefix = config_url_prefix,
            .avatar = chat.account.avatar,
            .acct = chat.account.acct,
            .messages = messages_html
        };

        chats_page = tmpl_gen_chat_view(&tmpl, len);
    }

    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&acct_storage);
    free(messages_html);
    // TODO cleanup messages
    return chats_page;
}

void content_chat_view(struct session* ssn, mastodont_t* api, char** data)
{
    char* chat_view = construct_chat_view(ssn, api, data[0], NULL);

    struct base_page b = {
        .category = BASE_CAT_CHATS,
        .content = chat_view,
        .sidebar_left = NULL
    };
    
    // Output
    render_base_page(&b, ssn, api);
    
    free(chat_view);
}


void content_chat_embed(struct session* ssn, mastodont_t* api, char** data)
{
    size_t result_len;
    char* result;
    char* chat_view = construct_chat_view(ssn, api, data[0], NULL);

    struct embed_template tmpl = {
        .stylesheet = "treebird20",
        .embed = chat_view,
    };

    result = tmpl_gen_embed(&tmpl, &result_len);
    
    // Output
    send_result(NULL, NULL, result, result_len);
    
    free(chat_view);
    free(result);
}


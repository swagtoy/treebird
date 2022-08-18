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
#include "account.h"
#include "emoji.h"
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

void content_chats(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_chat* chats = NULL;
    size_t chats_len = 0;
    struct mstdnt_storage storage = { 0 };

    struct mstdnt_chats_args args = {
        .with_muted = MSTDNT_TRUE,
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = keyint(ssn->query.offset),
        .limit = 20,
    };

    mastodont_get_chats_v2(api, &m_args, &args, &storage, &chats, &chats_len);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (chats)
        XPUSHs(newRV_noinc((SV*)perlify_chats(chats, chats_len)));
    else ARG_UNDEFINED();

    PERL_STACK_SCALAR_CALL("chat::content_chats");

    // Duplicate so we can free the TMPs
    char* dup = PERL_GET_STACK_EXIT;

    struct base_page b = {
        .category = BASE_CAT_CHATS,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };
    
    // Outpuot
    render_base_page(&b, req, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    // TOOD cleanup chats
    Safefree(dup);
}

void content_chat_view(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_message* messages = NULL;
    /* struct mstdnt_account acct; */
    size_t messages_len = 0;
    struct mstdnt_storage storage = { 0 }, storage_chat = { 0 };
    struct mstdnt_chat chat;

    struct mstdnt_chats_args args = {
        .with_muted = MSTDNT_TRUE,
        .max_id = keystr(ssn->post.max_id),
        .since_id = NULL,
        .min_id = keystr(ssn->post.min_id),
        .offset = keyint(ssn->query.offset),
        .limit = 20,
    };
    
    mastodont_get_chat_messages(api, &m_args, data[0], &args, &storage, &messages, &messages_len);
    int chat_code = mastodont_get_chat(api, &m_args, data[0],
                       &storage_chat, &chat);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (chat_code == 0)
        XPUSHs(newRV_noinc((SV*)perlify_chat(&chat)));
    else ARG_UNDEFINED();
    if (messages)
        XPUSHs(newRV_noinc((SV*)perlify_messages(messages, messages_len)));
    else ARG_UNDEFINED();
    
    PERL_STACK_SCALAR_CALL("chat::construct_chat");

    // Duplicate so we can free the TMPs
    char* dup = PERL_GET_STACK_EXIT;

    struct base_page b = {
        .category = BASE_CAT_CHATS,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };
    
    // Output
    render_base_page(&b, req, ssn, api);

    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&storage_chat);
    Safefree(dup);
}

HV* perlify_chat(const struct mstdnt_chat* chat)
{
    if (!chat) return NULL;

    HV* chat_hv = newHV();
    hvstores_ref(chat_hv, "account", perlify_account(&(chat->account)));
    hvstores_str(chat_hv, "id", chat->id);
    hvstores_int(chat_hv, "unread", chat->unread);

    return chat_hv;
}

AV* perlify_chats(const struct mstdnt_chat* chats, size_t len)
{
    if (!(chats && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_chat(chats + i)));
    }

    return av;
}

HV* perlify_message(const struct mstdnt_message* message)
{
    if (!message) return NULL;

    HV* message_hv = newHV();
    hvstores_str(message_hv, "account_id", message->account_id);
    hvstores_str(message_hv, "chat_id", message->chat_id);
    hvstores_str(message_hv, "id", message->id);
    hvstores_str(message_hv, "content", message->content);
    hvstores_int(message_hv, "created_at", message->created_at);
    hvstores_ref(message_hv, "emojis", perlify_emojis(message->emojis, message->emojis_len));
    hvstores_int(message_hv, "unread", message->unread);

    return message_hv;
}

AV* perlify_messages(const struct mstdnt_message* messages, size_t len)
{
    if (!(messages && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_message(messages + i)));
    }

    return av;
}


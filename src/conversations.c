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

    mstdnt_get_chats_v2(api, &m_args, &args, &storage, &chats, &chats_len);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (chats)
        mXPUSHs(newRV_noinc((SV*)perlify_chats(chats, chats_len)));
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
    mstdnt_storage_cleanup(&storage);
    mstdnt_cleanup_chats(chats, chats_len);
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
    
    mstdnt_get_chat_messages(api, &m_args, data[0], &args, &storage, &messages, &messages_len);
    int chat_code = mstdnt_get_chat(api, &m_args, data[0],
                       &storage_chat, &chat);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (chat_code == 0)
        mXPUSHs(newRV_noinc((SV*)perlify_chat(&chat)));
    else ARG_UNDEFINED();
    if (messages)
        mXPUSHs(newRV_noinc((SV*)perlify_messages(messages, messages_len)));
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

    mstdnt_storage_cleanup(&storage);
    mstdnt_storage_cleanup(&storage_chat);
    mstdnt_cleanup_chat(&chat);
    mstdnt_cleanup_messages(messages);
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

PERLIFY_MULTI(chat, chats, mstdnt_chat)

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

PERLIFY_MULTI(message, messages, mstdnt_message)

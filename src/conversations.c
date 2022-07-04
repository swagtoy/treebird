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

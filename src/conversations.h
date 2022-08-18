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

#ifndef CONVERSATIONS_H
#define CONVERSATIONS_H
#include "path.h"
#include <stddef.h>
#include <mastodont.h>
#include "session.h"

char* construct_chat(mastodont_t* api,
                     struct mstdnt_args* m_args,
                     struct mstdnt_chat* chat,
                     size_t* size);
char* construct_chats(mastodont_t* api,
                      struct mstdnt_args* m_args,
                      struct mstdnt_chat* chats,
                      size_t size,
                      size_t* ret_size);
char* construct_chats_view(char* lists_string, size_t* size);
// Message
char* construct_message(struct mstdnt_message* message,
                        struct mstdnt_account* your_profile,
                        struct mstdnt_account* their_profile,
                        size_t* size);
char* construct_messages(struct mstdnt_message* message,
                         struct mstdnt_account* your_profile,
                         struct mstdnt_account* their_profile,
                         size_t size,
                         size_t* ret_size);

void content_chats(PATH_ARGS);
void content_chat_view(PATH_ARGS);

AV* perlify_chats(const struct mstdnt_chat* chats, size_t chats_len);
HV* perlify_chat(const struct mstdnt_chat* chat);

AV* perlify_messages(const struct mstdnt_message* messages, size_t messages_len);
HV* perlify_message(const struct mstdnt_message* message);

#endif // LISTS_H

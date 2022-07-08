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

void content_chats(struct session* ssn, mastodont_t* api, char** data);
char* construct_chat_view(struct session* ssn, mastodont_t* api, char* id, size_t* len);
void content_chat_embed(struct session* ssn, mastodont_t* api, char** data);
void content_chat_view(struct session* ssn, mastodont_t* api, char** data);

#endif // LISTS_H

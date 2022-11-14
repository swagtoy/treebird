/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef CONVERSATIONS_H
#define CONVERSATIONS_H
#include "path.h"
#include <stddef.h>
#include <mastodont.h>
#include "session.h"

int content_chats(PATH_ARGS);
int content_chat_view(PATH_ARGS);

AV* perlify_chats(const struct mstdnt_chat* chats, size_t chats_len);
HV* perlify_chat(const struct mstdnt_chat* chat);

AV* perlify_messages(const struct mstdnt_message* messages, size_t messages_len);
HV* perlify_message(const struct mstdnt_message* message);

#endif // LISTS_H

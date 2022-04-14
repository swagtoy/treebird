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

#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <stddef.h>
#include <mastodont.h>
#include "session.h"

enum account_tab
{
    ACCT_TAB_STATUSES,
    ACCT_TAB_SCROBBLES,
    ACCT_TAB_PINNED,
    ACCT_TAB_MEDIA
};

char* construct_account_page(mastodont_t* api,
                             struct mstdnt_account* acct,
                             struct mstdnt_relationship* relationship,
                             enum account_tab tab,
                             char* content,
                             size_t* res_size);

char* construct_account_info(struct mstdnt_account* acct,
                             size_t* size);

void content_account_statuses(struct session* ssn, mastodont_t* api, char** data);

void content_account_scrobbles(struct session* ssn, mastodont_t* api, char** data);

void content_account_pinned(struct session* ssn, mastodont_t* api, char** data);

void content_account_media(struct session* ssn, mastodont_t* api, char** data);

void content_account_action(struct session* ssn, mastodont_t* api, char** data);
#endif // ACCOUNT_H

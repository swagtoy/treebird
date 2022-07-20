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

#ifndef SEARCH_H
#define SEARCH_H
#include <mastodont.h>
#include "session.h"
#include "path.h"

enum search_tab
{
    SEARCH_STATUSES,
    SEARCH_ACCOUNTS,
    SEARCH_HASHTAGS,
};

void search_page(FCGX_Request* req,
                 struct session* ssn,
                 mastodont_t* api,
                 enum search_tab tab,
                 char* content);
void content_search_all(PATH_ARGS);
void content_search_statuses(PATH_ARGS);
void content_search_accounts(PATH_ARGS);
void content_search_hashtags(PATH_ARGS);

#endif /* SEARCH_H */

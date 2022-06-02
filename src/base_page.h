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

#ifndef BASE_PAGE_H
#define BASE_PAGE_H
#include <mastodont.h>
#include "l10n.h"
#include "local_config.h"
#include "session.h"

enum base_category
{
    BASE_CAT_NONE,
    BASE_CAT_HOME,
    BASE_CAT_LOCAL,
    BASE_CAT_FEDERATED,
    BASE_CAT_NOTIFICATIONS,
    BASE_CAT_LISTS,
    BASE_CAT_FAVOURITES,
    BASE_CAT_BOOKMARKS,
    BASE_CAT_DIRECT,
    BASE_CAT_CONFIG,
};

struct base_page
{
    enum base_category category;
    enum l10n_locale locale;
    char* content;
    char* sidebar_left;
};

void render_base_page(struct base_page* page, struct session* ssn, mastodont_t* api);

/**
 * Outputs HTML in format for CGI. This can only be called once!
 *
 * @param status The full HTTP status. if NULL, then status is "200 OK"
 * @param content_type The Content-Type to display. if NULL, assume "text/html"
 * @param data HTML content
 * @param data_len Length of data. If 0, calls strlen(data)
 */
void send_result(char* status, char* content_type, char* data, size_t data_len);

#endif // BASE_PAGE_H

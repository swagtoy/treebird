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

#endif // BASE_PAGE_H

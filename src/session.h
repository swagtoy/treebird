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

#ifndef SESSION_H
#define SESSION_H
#include <mastodont.h>
#include "query.h"
#include "local_config.h"
#include "cookie.h"

struct session
{
    struct post_values post;
    struct get_values query;
    struct cookie_values cookies;
    struct local_config config;
    int logged_in;
    struct mstdnt_account acct;
    struct mstdnt_storage acct_storage;
};

#endif // SESSION_H

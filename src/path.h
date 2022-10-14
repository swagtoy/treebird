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

#ifndef PATH_H
#define PATH_H
#include "session.h"
#include <mastodont.h>
#include <stddef.h>
#include "env.h"
#include "cgi.h"
#include "request.h"

#define PATH_ARGS REQUEST_T req, struct session* ssn, mstdnt_t* api, char** data

struct path_info
{
    char* path;
    void (*callback)(PATH_ARGS);
};

void handle_paths(
    REQUEST_T req,
    struct session* ssn,
    mstdnt_t* api,
    struct path_info* paths,
    size_t paths_len);

int parse_path(REQUEST_T req,
               struct session* ssn,
               mstdnt_t* api,
               struct path_info* path_info);

#endif // PATH_H

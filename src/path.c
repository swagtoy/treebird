/*
 * RatFE - Lightweight frontend for Pleroma
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

#include <string.h>
#include <stdlib.h>
#include "path.h"

void handle_paths(mastodont_t* api, struct path_info* paths, size_t paths_len)
{
    char* path = getenv("PATH_INFO");
    // "default" path
    if (path == NULL || (path && strcmp(path, "/") == 0))
    {
        content_index(api);
    }
    else if (path && path[1] == '@')
    {   // Account path
        content_index(api);
    }
    else if (path)
    {   // Generic path
        for (size_t i = 0; i < paths_len; ++i)
        {
            if (strcmp(path, paths[i].path) == 0)
                paths[i].callback(api);
        }
    }
}

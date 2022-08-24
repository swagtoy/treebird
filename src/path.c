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

#include <string.h>
#include <stdlib.h>
#include "path.h"
#include "index.h"
#include "account.h"
#include "error.h"

int parse_path(REQUEST_T req,
               struct session* ssn,
               mastodont_t* api,
               struct path_info* path_info)
{
    int res = 0;
    int fail = 0, fin = 0;
    char* p = path_info->path + 1;
    char* p2 = GET_ENV("PATH_INFO", req) + 1;

    // Stored into data
    char* tmp = NULL;
    char** data = NULL;
    size_t size = 0;

    char* after_str = 0;
    size_t read_len;
    
    for (int i = 0, j = 0; p[j] || p2[i]; (++i, ++j))
        switch (p[j])
        {
        case ':':
            // TODO null check
            if (p[j+1] == '\0')
            {
                after_str = strchr(p2 + i, '/');
                if (!after_str)
                    after_str = p2+i + strlen(p2+i);
                fin = 1;
            }
            else
                after_str = strstr(p2 + i, "/");
            if (!after_str)
            {
                fail = 1;
                goto breakpt;
            }

            read_len = (size_t)after_str - (size_t)(p2 + i);
            // Copy in new data from the string we just read
            tmp = malloc(read_len+1);
            strncpy(tmp, after_str - read_len, read_len);
            tmp[read_len] = '\0';
            // Add our new string
            data = realloc(data, ++size * sizeof(tmp));
            data[size-1] = tmp;
            // Move ahead (-1 because we move again)
            i += read_len - 1;
            if (fin) goto breakpt;
            break;
        default:
            if (p2[i] == '/' && p[j] == '\0') goto breakpt;
            if (p[j] != p2[i])
            {
                fail = 1;
                goto breakpt;
            }
            break;
        }
breakpt:
    if (!fail)
    {
        path_info->callback(req, ssn, api, data);
    }
    else
    {
        res = 1;
    }

    // Cleanup
    for (size_t i = 0; i < size; ++i)
    {
        free(data[i]);
    }
    if (data) free(data);
    return res;
}

void handle_paths(REQUEST_T req,
                  struct session* ssn,
                  mastodont_t* api,
                  struct path_info* paths,
                  size_t paths_len)
{
    char* path = GET_ENV("PATH_INFO", req);
    // "default" path
    if (path == NULL || (path && strcmp(path, "/") == 0))
    {
        content_index(req, ssn, api);
    }
    else {   // Generic path
        for (size_t i = 0; i < paths_len; ++i)
        {
            if (parse_path(req, ssn, api, paths + i) == 0)
                return;
        }

        // Fell out, return 404
        content_not_found(req, ssn, api, path);
    }
}

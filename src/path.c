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

enum path_state
{
    PARSE_NEUTRAL,
    PARSE_READ,
};

int parse_path(struct session* ssn,
               mastodont_t* api,
               struct path_info* path_info)
{
    int fail = 0, fin = 0;
    enum path_state state = PARSE_NEUTRAL;
    char* p = path_info->path + 1;
    char* p2 = getenv("PATH_INFO") + 1;

    // Stored into data
    int str_size = 0;
    char* tmp = NULL;
    char** data = NULL;
    size_t size = 0;
    
    for (int i = 0, j = 0;;)
    {
        switch (p[j])
        {
        case '\0':
            fin = 1;
            // fall
        case '/':
            if (state == PARSE_READ)
            {
                state = PARSE_NEUTRAL;

                // Set value and move on
                data = realloc(data, ++size * sizeof(tmp));
                data[size-1] = tmp;
                tmp = NULL;
                str_size = 0;
            }
            else if (state == PARSE_NEUTRAL && fin != 1) {
                if (p[j] == p2[i])
                    break;
                else {
                    fail = 1;
                    goto breakpt;
                }
            }
            
            if (fin) goto breakpt;
            break;
        case ':':
            state = PARSE_READ;
            /* Abort early */
            if (p2[j] == '\0')
            {
                fail = 1;
                goto breakpt;
            }
            // fall
        default:
            if (state == PARSE_NEUTRAL)
            {
                if (p[j] == p2[i])
                    break;
                else {
                    fail = 1;
                    goto breakpt;
                }
            }
            else {
                // Don't realloc, we already have a space for our final character
                if (p2[i] == '\0' || p2[i] == '/')
                {
                    tmp[str_size] = '\0';
                    ++j;
                    // Move --i back one to counter the upcoming ++i
                    // If we don't, then we are one step too far
                    --i;
                }
                else {
                    tmp = realloc(tmp, ++str_size + 1);
                    tmp[str_size-1] = p2[i];
                }
            }
                    
                
            break;
        }

        if (state == PARSE_NEUTRAL) ++j; // Used for p
        ++i; // Used for p2
    }
breakpt:
    if (fail)
        return 1;

    path_info->callback(ssn, api, data);

    // Cleanup
    for (size_t i = 0; i < size; ++i)
    {
        free(data[i]);
    }
    if (data) free(data);
    return 0;
}

void handle_paths(struct session* ssn,
                  mastodont_t* api,
                  struct path_info* paths,
                  size_t paths_len)
{
    char* path = getenv("PATH_INFO");
    // "default" path
    if (path == NULL || (path && strcmp(path, "/") == 0))
    {
        content_index(ssn, api);
    }
    else {   // Generic path
        for (size_t i = 0; i < paths_len; ++i)
        {
            if (parse_path(ssn, api, paths + i) == 0)
                return;
        }

        // Fell out, return 404
        content_not_found(ssn, api, path);
    }
}

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

#include <fcgi_stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mime.h"

char* get_mime_boundary()
{
    if (!getenv("CONTENT_TYPE")) return 1;

    // Data gets changed in place
    char* content_type = malloc(strlen(getenv("CONTENT_TYPE"))+1);
    if (!content_type)
    {
        perror("malloc");
        exit(1);
    }

    char* bound_str;
    char* boundary;

    /* Tmp reading variables */
    char* tmp;

    if (strstr(content_type, "multipart/form-data") == NULL ||
        (bound_str = strstr(content_type, "boundary")) == NULL)
        goto error;

    bound_str += sizeof("boundary")-1;

    boundary = (tmp = strchr(bound_str, '\"')) ? tmp :
        strchr(bound_str, '=');
    if (!boundary)
        goto error;
    boundary++;

    if ((tmp = strchr(boundary, '\"')))
        *tmp = '\0';

    return 0;
error:
    free(content_type);
    return 1;
}

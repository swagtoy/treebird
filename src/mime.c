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

#include <ctype.h>
#include <fcgi_stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mime.h"

char* get_mime_boundary(char* content_type_str, char** bound)
{
    char* content = content_type_str ? content_type_str : getenv("CONTENT_TYPE");

    // Data gets changed in place
    char* content_type = malloc(strlen(content)+1);
    if (!content_type)
    {
        perror("malloc");
        exit(1);
    }
    strcpy(content_type, content);

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

    *bound = boundary;
    
    return content_type;
error:
    free(content_type);
    return NULL;
}

char* strnws(char* str)
{
    for (; isblank(*str); ++str);
    return str;
}

#define STRSCMP(begin, str) strncmp((begin), (str), sizeof(str))
char* read_form_data(char* boundary, char* begin, struct http_form_info* info)
{
    char* r, *name;
    size_t bound_len = strlen(boundary);
    if (strncmp(begin, "--", 2) != 0 ||
        strncmp(begin+2, boundary, bound_len) != 0)
        return NULL;

    r = begin + 2 + bound_len + 2; /* Skip over LRSF */

    if (STRSCMP(r, "Content-Disposition") != 0)
        return NULL;

    r = strnws(r + sizeof("Content-Disposition")+1);
    if (*r != ':' ||
        STRSCMP(r, "form-data") != 0) // Fucked up
        return NULL;
    
    r = strnws(r + sizeof("form-data")+1);
    if (*r != ';')
        return NULL;
    r = strnws(r+1);
    if (STRSCMP(r, "name") != 0)
        return NULL;
    r = strnws(r + sizeof("name")+1);
    if (*r != '=')
        return NULL;
    r = strnws(r + 1);
    if (*r == '\"')
        r += 2;
    if (*r == '\"') ++r;
    name = r;
    if ((r = strchr(r, '\"')))
        *r = '\0';

    printf("nameparsed: %s\r\n", name);

    return name;
}

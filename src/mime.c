/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <ctype.h>
#include "global_perl.h"
#include <fcgi_stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "mime.h"
#include "string.h"

char*
get_mime_boundary(char* content_type_str, char** bound)
{
    // If neither values are set, get out
    if (!content_type_str)
        return NULL;
    
    char* content = content_type_str;

    // Data gets changed in place
    char* content_type = tb_malloc(strlen(content)+1);
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
    tb_free(content_type);
    return NULL;
}

static char*
strnws(char* str)
{
    for (; isblank(*str); ++str);
    return str;
}

// Function assumes character is at ';' char
static char*
read_key(char** r, char* key)
{
    char* val, *read_val;
    size_t key_len = strlen(key);
    *r = strnws(*r+1);
    if (strncmp(*r, key, key_len) != 0)
        return NULL;
    *r = strnws(*r + key_len);
    if (**r != '=')
        return NULL;
    *r = strnws(*r + 1);
    if (**r == '\"') ++(*r);
    val = *r;
    // Chop off string
    if ((read_val = strchr(*r, '\"')) ||
        (read_val = strchr(*r, ';')) ||
        (read_val = strchr(*r, '\r')))
        *read_val = '\0';
    // Jump to end
    *r += strlen(val)+1;

    return val;
}

#define STRSCMP(begin, str) strncmp((begin), (str), sizeof(str)-1)

char* read_form_data(char* boundary,
                     char* begin,
                     struct http_form_info* info,
                     size_t size)
{
    ptrdiff_t data_size, begin_to_value_size;
    // Step 1: Parse name
    // Parser variables
    char* r, *read_val;
    size_t bound_len = strlen(boundary);
    if (strncmp(begin, "--", 2) != 0 ||
        strncmp(begin+2, boundary, bound_len) != 0)
        return NULL;

    // Check if this is the end
    if (strncmp(begin + 2 + bound_len, "--", 2) == 0)
        return NULL;

    r = begin + 2 + bound_len + 2; /* Skip over LRSF */

    if (STRSCMP(r, "Content-Disposition") != 0)
        return NULL;

    r = strnws(r + sizeof("Content-Disposition")-1);
    if (*r != ':') // Fucked up
        return NULL;
    
    r = strnws(r + 1);
    if (STRSCMP(r, "form-data") != 0)
        return NULL;
    r = strnws(r + sizeof("form-data")-1);
    if (*r != ';')
        return NULL;
    
    info->name = read_key(&r, "name");
    // Step 2: Parse filename (if there)
    if (*r == ';')
    {
        info->filename = read_key(&r, "filename");
    }
    else
        info->filename = NULL;

    if (*r == '\r')
        r += 2;

    // Step 3: Parse Content-type (if there)
    if (STRSCMP(r, "Content-Type") == 0)
    {
        r = strnws(r + sizeof("Content-type")-1);
        if (*r != ':')
            return NULL;

        r = strnws(r + 1);
        info->content_type = r;
        if ((r = strchr(r, '\r')))
            *r = '\0';
        else
            return NULL;
        r += 4;
    }
    else if (*r == '\r') {
        info->content_type = NULL;
        r += 2;
    }

    // Last step: Find data
    info->value = r;
    begin_to_value_size = info->value - begin;

    // Look for end
    if ((r = strnstr(r, boundary, size - begin_to_value_size)) &&
        r && strnstr(r-4, "\r\n--", size - begin_to_value_size))
    {
        r[-4] = '\0';
        data_size = (r-4) - info->value;
        info->value_size = data_size;
    }
    else
        return NULL;

    // Go back for next read
    r -= 2;

    return r;
}

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef MIME_H
#define MIME_H
#include <stddef.h>

struct http_form_info
{
    char* name;
    char* filename;
    char* content_type;
    char* value;
    size_t value_size;
};

char* get_mime_boundary(char* content_type, char** res);
char* read_form_data(char* boundary,
                     char* begin,
                     struct http_form_info* info,
                     size_t size);

#endif /* MIME_H */

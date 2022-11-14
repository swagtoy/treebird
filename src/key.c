/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#include "global_perl.h"
#include <stdlib.h>
#include <limits.h>
#include "key.h"

void key_string(char* val, struct file_content* props, struct key* arg)
{
    arg->type.s = val;
}

void key_int(char* val, struct file_content* form, struct key* arg)
{
    char* err;

    // Convert
    long result = strtol(val, &err, 10);
    if (err == val ||
        // Overflow
        result == LONG_MIN || result == LONG_MAX)
    {
        arg->type.i = 0;
        return;
    }
    arg->type.i = result;
}

void key_files(char* val, struct file_content* form, struct key* arg)
{
    struct file_array* arr = &(arg->type.f);
    char* content_cpy;

    arr->content = tb_realloc(arr->content,
                           sizeof(struct file_content) * ++(arr->array_size));
    if (!(arr->content))
        return;

    // Make a copy so we can remember it later
    if (!(content_cpy = tb_malloc(form->content_size+1)))
        return;
    
    memcpy(content_cpy, val, form->content_size+1);

    // Store
    arr->content[arr->array_size-1].content = content_cpy;
    arr->content[arr->array_size-1].content_size = form->content_size;
    arr->content[arr->array_size-1].filename = form->filename;
    arr->content[arr->array_size-1].filetype = form->filetype;
}

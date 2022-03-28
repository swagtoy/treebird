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

#include <stdlib.h>
#include "easprintf.h"
#include "attachments.h"
#include "string_helpers.h"

// Pages
#include "../static/attachments.chtml"
#include "../static/attachment_image.chtml"

struct attachments_args
{
    struct mstdnt_attachment* atts;
    mstdnt_bool sensitive;
};

char* construct_attachment(mstdnt_bool sensitive, struct mstdnt_attachment* att, int* str_size)
{
    char* att_html;

    size_t s = easprintf(&att_html, data_attachment_image_html,
                         sensitive ? "sensitive" : "",
                         att->url);
    if (str_size) *str_size = s;
    return att_html;
}

static char* construct_attachments_voidwrap(void* passed, size_t index, int* res)
{
    struct attachments_args* args = passed;
    return construct_attachment(args->sensitive, args->atts + index, res);
}

char* construct_attachments(mstdnt_bool sensitive, struct mstdnt_attachment* atts, size_t atts_len, size_t* str_size)
{
    size_t elements_size;
    struct attachments_args args = { atts, sensitive };
    char* elements = construct_func_strings(construct_attachments_voidwrap, &args, atts_len, &elements_size);
    char* att_view;

    size_t s = easprintf(&att_view, data_attachments_html, elements);
    if (str_size) *str_size = s;
    // Cleanup
    free(elements);
    return att_view;
}

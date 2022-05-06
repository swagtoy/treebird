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

#include "hashtag.h"
#include "string_helpers.h"
#include "easprintf.h"

// Pages
#include "../static/hashtag.chtml"
#include "../static/hashtag_page.chtml"
#include "../static/hashtag_statistics.chtml"

#define TAG_SIZE_INITIAL 12

char* construct_hashtag(struct mstdnt_tag* hashtag, int* size)
{
    char* hashtag_html;

    // Lol!
    unsigned hash_size = TAG_SIZE_INITIAL +
        ((unsigned)(
            (hashtag->history && hashtag->history_len >= 1 ?
             hashtag->history[0].uses : 0) + 4
            /4)*2);

    size_t s = easprintf(&hashtag_html, data_hashtag_html,
                         hash_size, hashtag->name);

    if (size) *size = s;
    return hashtag_html;
}

static char* construct_hashtag_voidwrap(void* passed, size_t index, int* res)
{
    return construct_hashtag((struct mstdnt_tag*)passed + index, res);
}
char* construct_hashtags(struct mstdnt_tag* hashtags, size_t size, size_t* ret_size)
{
    if (!(hashtags && size)) return NULL;
    return construct_func_strings(construct_hashtag_voidwrap, hashtags, size, ret_size);
}


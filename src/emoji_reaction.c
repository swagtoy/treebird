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

#include "string_helpers.h"
#include "emoji_reaction.h"
#include <stdlib.h>
#include "easprintf.h"

// Templates
#include "../static/emoji_reaction.chtml"
#include "../static/emoji_reactions.chtml"

char* construct_emoji_reaction(struct mstdnt_emoji_reaction* emo, int* str_size)
{
    char* emo_html;

    size_t s = easprintf(&emo_html, data_emoji_reaction_html,
                         emo->name, emo->count);
    if (str_size) *str_size = s;
    return emo_html;
}

static char* construct_emoji_reactions_voidwrap(void* passed, size_t index, int* res)
{
    return construct_emoji_reaction((struct mstdnt_emoji_reaction*)passed + index, res);
}

char* construct_emoji_reactions(struct mstdnt_emoji_reaction* emos, size_t emos_len, size_t* str_size)
{
    size_t elements_size;
    char* elements = construct_func_strings(construct_emoji_reactions_voidwrap, emos, emos_len, &elements_size);
    char* emos_view;

    size_t s = easprintf(&emos_view, data_emoji_reactions_html, elements);
    if (str_size) *str_size = s;
    // Cleanup
    free(elements);
    return emos_view;
}


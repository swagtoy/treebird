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

#include "../config.h"
#include "string_helpers.h"
#include "emoji_reaction.h"
#include <stdlib.h>
#include "easprintf.h"

// Templates
#include "../static/emoji_reaction.ctmpl"
#include "../static/emoji_reactions.ctmpl"

struct construct_emoji_reactions_args
{
    struct mstdnt_emoji_reaction* emojis;
    char* id;
};

char* construct_emoji_reaction(char* id, struct mstdnt_emoji_reaction* emo, size_t* str_size)
{
    struct emoji_reaction_template data = {
        .prefix = config_url_prefix,
        .status_id = id,
        .emoji = emo->name,
        .emoji_active = emo->me ? "active" : NULL,
        .emoji_count = emo->count
    };
    return tmpl_gen_emoji_reaction(&data, str_size);
}

static char* construct_emoji_reactions_voidwrap(void* passed, size_t index, size_t* res)
{
    struct construct_emoji_reactions_args* args = passed;
    return construct_emoji_reaction(args->id, args->emojis + index, res);
}

char* construct_emoji_reactions(char* id, struct mstdnt_emoji_reaction* emos, size_t emos_len, size_t* str_size)
{
    size_t elements_size;
    struct construct_emoji_reactions_args args = {
        .emojis = emos,
        .id = id
    };
    char* elements = construct_func_strings(construct_emoji_reactions_voidwrap, &args, emos_len, &elements_size);
    char* emos_view;

    struct emoji_reactions_template data = {
        .emojis = elements
    };
    emos_view = tmpl_gen_emoji_reactions(&data, str_size);
    // Cleanup
    free(elements);
    return emos_view;
}


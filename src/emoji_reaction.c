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


struct construct_emoji_reactions_args
{
    struct mstdnt_emoji_reaction* emojis;
    char* id;
};

char* construct_emoji_reaction(char* id, struct mstdnt_emoji_reaction* emo, size_t* str_size)
{
    char* ret;
    char* emoji = emo->name;
    if (emo->url)
    {
        struct custom_emoji_reaction_template c_data = {
            .url = emo->url,
        };
        emoji = tmpl_gen_custom_emoji_reaction(&c_data, NULL);
    }
    
    struct emoji_reaction_template data = {
        .prefix = config_url_prefix,
        .status_id = id,
        .custom_emoji = emo->url ? "custom-emoji-container" : NULL,
        .emoji = emo->name,
        .emoji_display = emoji,
        .emoji_active = emo->me ? "active" : NULL,
        .emoji_count = emo->count
    };

    ret = tmpl_gen_emoji_reaction(&data, str_size);
    if (emoji != emo->name)
        free(emoji);
    return ret;
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

HV* perlify_emoji_reaction(struct mstdnt_emoji_reaction* const emoji)
{
    if (!emoji) return NULL;
    HV* emoji_hv = newHV();
    hvstores_str(emoji_hv, "name", emoji->name);
    hvstores_str(emoji_hv, "url", emoji->url);
    hvstores_str(emoji_hv, "static_url", emoji->static_url);
    hvstores_int(emoji_hv, "count", emoji->count);
    hvstores_int(emoji_hv, "me", emoji->me);
    return emoji_hv;
}

AV* perlify_emoji_reactions(struct mstdnt_emoji_reaction* const emos, size_t len)
{
    if (!(emos && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_emoji_reaction(emos + i)));
    }

    return av;
}

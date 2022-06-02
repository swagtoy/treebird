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
#include <string.h>
#include "string.h"
#include "emoji.h"
#include "easprintf.h"
#include "string_helpers.h"

// Pages
#include "../static/emoji.ctmpl"
#include "../static/emoji_picker.ctmpl"

char* emojify(char* content, struct mstdnt_emoji* emos, size_t emos_len)
{
    if (!content) return NULL;
    size_t sc_len;
    char* oldres = NULL;
    char* res = content;
    char* emoji_url_str;
    char* coloned;
    for (size_t i = 0; i < emos_len; ++i)
    {
        oldres = res;

        // Add colons around string
        sc_len = strlen(emos[i].shortcode);
        // 3 = \0 and two :
        coloned = malloc(sc_len+3);
        coloned[0] = ':';
        strncpy(coloned + 1, emos[i].shortcode, sc_len);
        coloned[sc_len+1] = ':';
        coloned[sc_len+2] = '\0';

        easprintf(&emoji_url_str, "<img class=\"emoji\" src=\"%s\">", emos[i].url);
        
        res = strrepl(res, coloned, emoji_url_str, STRREPL_ALL);
        if (oldres != content && res != oldres) free(oldres);
        // Cleanup
        free(emoji_url_str);
        free(coloned);
    }
    return res;
}

struct construct_emoji_picker_args
{
    char* status_id;
    unsigned index;
};

char* construct_emoji(struct emoji_info* emoji, char* status_id, size_t* size)
{
    if (!emoji)
        return NULL;

    struct emoji_template data = {
        .status_id = status_id,
        .emoji = emoji->codes
    };
    return tmpl_gen_emoji(&data, size);
}

static char* construct_emoji_voidwrap(void* passed, size_t index, size_t* res)
{
    struct construct_emoji_picker_args* args = passed;
    size_t calc_index = index + args->index;
    return calc_index < 0 || calc_index >= emojos_size ? NULL :
        construct_emoji(emojos + calc_index, args->status_id, res);
}

char* construct_emoji_picker(char* status_id, unsigned index, size_t* size)
{
    struct construct_emoji_picker_args args = {
        .status_id = status_id,
        .index = index
    };
    char* emoji_picker_html;
    char* emojis;

    emojis = construct_func_strings(construct_emoji_voidwrap, &args, EMOJI_FACTOR_NUM, NULL);
    
    struct emoji_picker_template data = {
        .cat_smileys = ACTIVE_CONDITION(index >= 0 && index < EMOJO_CAT_ANIMALS),
        .animals = EMOJO_CAT_ANIMALS,
        .cat_animals = ACTIVE_CONDITION(index >= EMOJO_CAT_ANIMALS && index < EMOJO_CAT_FOOD),
        .food = EMOJO_CAT_FOOD,
        .cat_food = ACTIVE_CONDITION(index >= EMOJO_CAT_FOOD && index < EMOJO_CAT_TRAVEL),
        .travel = EMOJO_CAT_TRAVEL,
        .cat_travel = ACTIVE_CONDITION(index >= EMOJO_CAT_TRAVEL && index < EMOJO_CAT_ACTIVITIES),
        .activities = EMOJO_CAT_ACTIVITIES,
        .cat_activities = ACTIVE_CONDITION(index >= EMOJO_CAT_ACTIVITIES && index < EMOJO_CAT_OBJECTS),
        .objects = EMOJO_CAT_OBJECTS,
        .cat_objects = ACTIVE_CONDITION(index >= EMOJO_CAT_OBJECTS && index < EMOJO_CAT_SYMBOLS),
        .symbols = EMOJO_CAT_SYMBOLS,
        .cat_symbols = ACTIVE_CONDITION(index >= EMOJO_CAT_SYMBOLS && index < EMOJO_CAT_FLAGS),
        .flags = EMOJO_CAT_FLAGS,
        .cat_flags = ACTIVE_CONDITION(index >= EMOJO_CAT_FLAGS && index < emojos_size),
        .emojis = emojis,
                         // Index movements
        .status_id = status_id,
        .index_previous = index > 0 ? index - EMOJI_FACTOR_NUM : 0,
        .previous_enabled = 0 > index - EMOJI_FACTOR_NUM ? "disabled" : "",
        .index_next = index + EMOJI_FACTOR_NUM
    };

    emoji_picker_html = tmpl_gen_emoji_picker(&data, size);
    free(emojis);
    return emoji_picker_html;
}

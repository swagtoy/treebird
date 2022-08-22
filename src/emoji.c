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

#include "emoji.h"
#include <stdlib.h>
#include <string.h>
#include "base_page.h"
#include "string.h"
#include "easprintf.h"
#include "string_helpers.h"

enum emoji_categories
{
    EMO_CAT_SMILEYS,
    EMO_CAT_ANIMALS,
    EMO_CAT_FOOD,
    EMO_CAT_TRAVEL,
    EMO_CAT_ACTIVITIES,
    EMO_CAT_OBJECTS,
    EMO_CAT_SYMBOLS,
    EMO_CAT_FLAGS,
    EMO_CAT_LEN
};

struct construct_emoji_picker_args
{
    char* status_id;
    unsigned index;
};

char* construct_emoji(struct emoji_info* emoji, char* status_id, size_t* size)
{
    if (!emoji)
        return NULL;
    char* emoji_str;

    if (status_id)
        *size = easprintf(&emoji_str, "<a href=\"/status/%s/react/%s\" class=\"emoji\">%s</a>",
                  status_id, emoji->codes, emoji->codes);
    else
        *size = easprintf(&emoji_str, "<span class=\"emoji\">%s</span>", emoji->codes);
    return emoji_str;
}

static char* construct_emoji_voidwrap(void* passed, size_t index, size_t* res)
{
    struct construct_emoji_picker_args* args = passed;
    size_t calc_index = index + args->index;
    return construct_emoji(emojos + calc_index, args->status_id, res);
}

#define EMOJI_PICKER_ARGS(this_index) { .status_id = status_id, .index = this_index }

void content_emoji_picker(PATH_ARGS)
{
    char* picker = construct_emoji_picker(NULL, NULL);

    send_result(req, NULL, NULL, picker, 0);

    free(picker);
}

char* construct_emoji_picker(char* status_id, size_t* size)
{
    
    char* emoji_picker_html;
    
    struct construct_emoji_picker_args args[EMO_CAT_LEN] = {
        EMOJI_PICKER_ARGS(EMOJO_CAT_SMILEY),
        EMOJI_PICKER_ARGS(EMOJO_CAT_ANIMALS),
        EMOJI_PICKER_ARGS(EMOJO_CAT_FOOD),
        EMOJI_PICKER_ARGS(EMOJO_CAT_TRAVEL),
        EMOJI_PICKER_ARGS(EMOJO_CAT_ACTIVITIES),
        EMOJI_PICKER_ARGS(EMOJO_CAT_OBJECTS),
        EMOJI_PICKER_ARGS(EMOJO_CAT_SYMBOLS),
        EMOJI_PICKER_ARGS(EMOJO_CAT_FLAGS),
    };
    
    char* emojis[EMO_CAT_LEN];
    size_t len[EMO_CAT_LEN];

    // TODO refactor to use #define lol
    emojis[EMO_CAT_SMILEYS] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_SMILEYS, EMOJO_CAT_ANIMALS - EMOJO_CAT_SMILEY, len);
    emojis[EMO_CAT_ANIMALS] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_ANIMALS, EMOJO_CAT_FOOD - EMOJO_CAT_ANIMALS, len + 1);
    emojis[EMO_CAT_FOOD] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_FOOD, EMOJO_CAT_TRAVEL - EMOJO_CAT_FOOD, len + 2);
    emojis[EMO_CAT_TRAVEL] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_TRAVEL, EMOJO_CAT_ACTIVITIES - EMOJO_CAT_TRAVEL, len + 3);
    emojis[EMO_CAT_ACTIVITIES] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_ACTIVITIES, EMOJO_CAT_OBJECTS - EMOJO_CAT_ACTIVITIES, len + 4);
    emojis[EMO_CAT_OBJECTS] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_OBJECTS, EMOJO_CAT_SYMBOLS - EMOJO_CAT_OBJECTS, len + 5);
    emojis[EMO_CAT_SYMBOLS] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_SYMBOLS, EMOJO_CAT_FLAGS - EMOJO_CAT_SYMBOLS, len + 6);
    emojis[EMO_CAT_FLAGS] = construct_func_strings(construct_emoji_voidwrap, args + EMO_CAT_FLAGS, EMOJO_CAT_MAX - EMOJO_CAT_FLAGS, len + 7);

    PERL_STACK_INIT;
    XPUSHs(newRV_noinc((SV*)template_files));
    AV* av = newAV();
    av_extend(av, EMO_CAT_LEN);
    for (int i = 0; i < EMO_CAT_LEN; ++i)
    {
        av_store(av, i, newSVpv(emojis[i], 0));
    }
    XPUSHs(newRV_noinc((SV*)av));
    PERL_STACK_SCALAR_CALL("emojis::emoji_picker");

    char* dup = PERL_GET_STACK_EXIT;

    // Cleanup
    for (size_t i = 0; i < EMO_CAT_LEN; ++i)
        free(emojis[i]);
    return dup;
}

HV* perlify_emoji(struct mstdnt_emoji* const emoji)
{
    if (!emoji) return NULL;
    HV* emoji_hv = newHV();
    hvstores_str(emoji_hv, "shortcode", emoji->shortcode);
    hvstores_str(emoji_hv, "url", emoji->url);
    hvstores_str(emoji_hv, "static_url", emoji->static_url);
    hvstores_int(emoji_hv, "visible_in_picker", emoji->visible_in_picker);
    hvstores_str(emoji_hv, "category", emoji->category);
    return emoji_hv;
}

AV* perlify_emojis(struct mstdnt_emoji* const emos, size_t len)
{
    if (!(emos && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_emoji(emos + i)));
    }

    return av;
}

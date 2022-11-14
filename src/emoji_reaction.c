/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include "../config.h"
#include "string_helpers.h"
#include "emoji_reaction.h"
#include <stdlib.h>
#include "easprintf.h"

HV* perlify_emoji_reaction(const struct mstdnt_emoji_reaction* const emoji)
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

PERLIFY_MULTI(emoji_reaction, emoji_reactions, mstdnt_emoji_reaction)


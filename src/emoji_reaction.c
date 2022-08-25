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


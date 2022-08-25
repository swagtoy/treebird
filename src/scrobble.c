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

#include "scrobble.h"
#include "easprintf.h"
#include "string_helpers.h"
#include "account.h"

// Converts it into a perl struct
HV* perlify_scrobble(const struct mstdnt_scrobble* const scrobble)
{
    if (!scrobble) return NULL;
    HV* scrobble_hv = newHV();

    hvstores_str(scrobble_hv, "album", scrobble->album);
    hvstores_str(scrobble_hv, "artist", scrobble->artist);
    hvstores_str(scrobble_hv, "id", scrobble->id);
    hvstores_str(scrobble_hv, "title", scrobble->title);
    hvstores_int(scrobble_hv, "created_at", scrobble->created_at);
    hvstores_int(scrobble_hv, "length", scrobble->created_at);
    hvstores_ref(scrobble_hv, "account", perlify_account(&(scrobble->account)));
    
    return scrobble_hv;
}

PERLIFY_MULTI(scrobble, scrobbles, mstdnt_scrobble)

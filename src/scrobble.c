/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
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

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

#include "../static/scrobble.ctmpl"

char* construct_scrobble(struct mstdnt_scrobble* scrobble, size_t* size)
{
    struct scrobble_template tdata = {
        .scrobble_id = scrobble->id,
        .avatar = scrobble->account.avatar,
        .username = scrobble->account.display_name,
        .activity = "is listening to...",
        .title_key = "Title",
        .title = scrobble->title,
        .artist_key = "Artist",
        .artist = scrobble->artist,
        .album_key = "Album",
        .album = scrobble->album,
        .length_key = "Duration",
        .length = scrobble->length
    };

    return tmpl_gen_scrobble(&tdata, size);
}

static char* construct_scrobble_voidwrap(void* passed, size_t index, int* res)
{
    return construct_scrobble((struct mstdnt_scrobble*)passed + index, res);
}

char* construct_scrobbles(struct mstdnt_scrobble* scrobbles, size_t scrobbles_len, size_t* ret_size)
{
    return construct_func_strings(construct_scrobble_voidwrap, scrobbles, scrobbles_len, ret_size);
}

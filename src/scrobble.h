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

#ifndef SCROBBLE_H
#define SCROBBLE_H
#include <mastodont.h>

char* construct_scrobble(struct mstdnt_scrobble* scrobble, size_t* size);
char* construct_scrobbles(struct mstdnt_scrobble* scrobbles, size_t scrobbles_len, size_t* ret_size);

#endif /* SCROBBLE_H */

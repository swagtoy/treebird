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

#ifndef GRAPHS_N_BARS_H
#define GRAPHS_N_BARS_H
#include <stddef.h>
#include <mastodont.h>
#include "session.h"

char* construct_bar_graph_container(char* bars, size_t* size);
char* construct_bar(float value, size_t* size);
char* construct_hashtags_graph(struct mstdnt_tag* tags,
                               size_t tags_len,
                               size_t days,
                               size_t* ret_size);

#endif /* GRAPHS_N_BARS_H */

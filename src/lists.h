/*
 * RatFE - Lightweight frontend for Pleroma
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

#ifndef LISTS_H
#define LISTS_H
#include <stddef.h>
#include <mastodont.h>

char* construct_list(struct mstdnt_list* list, int* size);
char* construct_lists(struct mstdnt_list* lists, size_t size, size_t* ret_size);
char* construct_lists_view(char* lists_string, int* size);
void content_lists(mastodont_t* api, char** data, size_t size);

#endif // LISTS_H

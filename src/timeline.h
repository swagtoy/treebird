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

#ifndef TIMELINE_H
#define TIMELINE_H
#include <stddef.h>
#include <mastodont.h>

// Federated and local are here
void tl_public(mastodont_t* api, int local);
void tl_list(mastodont_t* api, char* list_id);

void content_tl_federated(mastodont_t* api, char** data, size_t data_size);
void content_tl_local(mastodont_t* api, char** data, size_t data_size);
void content_tl_list(mastodont_t* api, char** data, size_t data_size);

#endif // TIMELINE_H

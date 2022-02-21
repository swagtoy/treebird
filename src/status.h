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

#ifndef STATUS_H
#define STATUS_H
#include <mastodont.h>

int try_post_status(mastodont_t* api);
int try_interact_status(mastodont_t* api);
char* construct_status(struct mstdnt_status* status, int* size);
char* construct_statuses(struct mstdnt_status* statuses, size_t size, size_t* ret_size);
void content_status(mastodont_t* api, char** data, size_t data_size);

#endif // STATUS_H

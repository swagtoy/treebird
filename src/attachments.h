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

#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H
#include <mastodont.h>
#include "session.h"

int try_upload_media(struct mstdnt_storage* storage,
                     struct session* ssn,
                     mastodont_t* api,
                     struct mstdnt_attachment** attachments,
                     char*** media_ids);
void cleanup_media_ids(struct session* ssn, char** media_ids);
char* construct_attachment(mstdnt_bool sensitive, struct mstdnt_attachment* att, int* str_size);
char* construct_attachments(mstdnt_bool sensitive, struct mstdnt_attachment* atts, size_t atts_len, size_t* str_size);

#endif // ATTACHMENTS_H

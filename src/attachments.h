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
#include "path.h"
#include "session.h"
#include "global_perl.h"

#define FILES_READY(ssn) (ssn->post.files.type.f.array_size &&          \
                          ssn->post.files.type.f.content &&             \
                          ssn->post.files.type.f.content[0].content_size)

int try_upload_media(struct mstdnt_storage** storage,
                     struct session* ssn,
                     mastodont_t* api,
                     struct mstdnt_attachment** attachments,
                     char*** media_ids);
void cleanup_media_storages(struct session* ssn, struct mstdnt_storage* storage);
void cleanup_media_ids(struct session* ssn, char** media_ids);
int api_attachment_create(PATH_ARGS);

// Perl
HV* perlify_attachment(const struct mstdnt_attachment* const attachment);
AV* perlify_attachments(const struct mstdnt_attachment* const attachments, size_t len);

#endif // ATTACHMENTS_H

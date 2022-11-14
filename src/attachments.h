/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
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

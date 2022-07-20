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

#ifndef TIMELINE_H
#define TIMELINE_H
#include <fcgi_stdio.h>
#include <fcgiapp.h>
#include <stddef.h>
#include <mastodont.h>
#include "path.h"
#include "session.h"
#include "base_page.h"

// Federated and local are here
void tl_home(FCGX_Request* req, struct session* ssn, mastodont_t* api, int local);
void tl_direct(FCGX_Request* req, struct session* ssn, mastodont_t* api);
void tl_public(FCGX_Request* req, struct session* ssn, mastodont_t* api, int local, enum base_category cat);
void tl_list(FCGX_Request* req, struct session* ssn, mastodont_t* api, char* list_id);
void tl_tag(FCGX_Request* req, struct session* ssn, mastodont_t* api, char* tag);

void content_tl_federated(PATH_ARGS);
void content_tl_home(PATH_ARGS);
void content_tl_direct(PATH_ARGS);
void content_tl_local(PATH_ARGS);
void content_tl_list(PATH_ARGS);
void content_tl_tag(PATH_ARGS);
void content_timeline(FCGX_Request* req,
                      struct session* ssn,
                      mastodont_t* api,
                      struct mstdnt_storage* storage,
                      struct mstdnt_status* statuses,
                      size_t statuses_len,
                      enum base_category cat,
                      char* header,
                      int show_post_box);

#endif // TIMELINE_H

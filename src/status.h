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

#ifndef STATUS_H
#define STATUS_H
#include <stdint.h>
#include <mastodont.h>
#include "session.h"

// Flags
#define STATUS_NOOP 0
#define STATUS_FOCUSED (1<<0)
#define STATUS_EMOJI_PICKER (1<<1)

char* construct_in_reply_to(mastodont_t* api, struct mstdnt_status* status, size_t* size);
int try_post_status(struct session* ssn, mastodont_t* api);
int try_interact_status(struct session* ssn, mastodont_t* api, char* id);
void content_status_create(struct session* ssn, mastodont_t* api, char** data);

// HTML Builders
char* construct_post_box(char* reply_id,
                         char* default_content,
                         int* size);
char* reformat_status(char* content);
char* construct_status(mastodont_t* api, struct mstdnt_status* status, int* size, struct mstdnt_notification* notif, uint8_t flags);
char* construct_statuses(mastodont_t* api, struct mstdnt_status* statuses, size_t size, size_t* ret_size);

// Status frontends
void status_view(struct session* ssn, mastodont_t* api, char** data);
void status_reply(struct session* ssn, mastodont_t* api, char** data);
void status_interact(struct session* ssn, mastodont_t* api, char** data);
// Above wraps to the below function
void content_status(struct session* ssn, mastodont_t* api, char** data, int is_reply);

// Cleanup
void cleanup_media_ids(struct session* ssn, char** media_ids);

#endif // STATUS_H

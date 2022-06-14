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
#include "l10n.h"
#include "session.h"

// Flags
#define STATUS_NOOP 0
#define STATUS_REPLY (1<<0)
#define STATUS_FOCUSED (1<<1)
#define STATUS_EMOJI_PICKER (1<<2)
#define STATUS_NO_LIKEBOOST (1<<3)
#define STATUS_NO_DOPAMEME (1<<4)

struct construct_statuses_args
{
    char* highlight_word;
};

struct interact_profile_args
{
    struct mstdnt_account* reblogs;
    struct mstdnt_account* favourites;
    size_t reblogs_len;
    size_t favourites_len;
};

int try_post_status(struct session* ssn, mastodont_t* api);
int try_interact_status(struct session* ssn, mastodont_t* api, char* id);
void content_status_create(struct session* ssn, mastodont_t* api, char** data);
void content_status_react(struct session* ssn, mastodont_t* api, char** data);

// HTML Builders
char* construct_status(struct session* ssn,
                       mastodont_t* api,
                       struct mstdnt_status* status,
                       size_t* size,
                       struct mstdnt_notification* notif,
                       struct construct_statuses_args* args,
                       uint8_t flags);
char* construct_statuses(struct session* ssn,
                       mastodont_t* api,
                         struct mstdnt_status* statuses,
                         size_t size,
                         struct construct_statuses_args* args,
                         size_t* ret_size);
char* construct_interaction_buttons(struct session* ssn,
                                    struct mstdnt_status* status,
                                    size_t* size,
                                    uint8_t flags);
// Reply to
char* get_in_reply_to(mastodont_t* api, struct mstdnt_status* status, size_t* size);
char* construct_in_reply_to(struct mstdnt_status* status,
                            struct mstdnt_account* account,
                            size_t* size);

char* construct_status_interactions(char* status_id,
                                    int fav_count,
                                    int reblog_count,
                                    struct mstdnt_account* fav_accounts,
                                    size_t fav_accounts_len,
                                    struct mstdnt_account* reblog_accounts,
                                    size_t reblog_accounts_len,
                                    size_t* size);

char* construct_status_interaction_profiles(struct mstdnt_account* reblogs,
                                            struct mstdnt_account* favourites,
                                            size_t reblogs_len,
                                            size_t favourites_len,
                                            size_t* ret_size);
char* construct_status_interaction_profile(struct interact_profile_args* args, size_t index, size_t* size);
char* construct_status_interactions_label(char* status_id,
                                          int is_favourites,
                                          char* header,
                                          int val,
                                          size_t* size);
char* reformat_status(struct session* ssn,
                      char* content,
                      struct mstdnt_emoji* emos,
                      size_t emos_len);
char* greentextify(char* content);
char* make_mentions_local(char* content);

void status_view_reblogs(struct session* ssn, mastodont_t* api, char** data);
void status_view_favourites(struct session* ssn, mastodont_t* api, char** data);

const char* status_visibility_str(enum l10n_locale locale, enum mstdnt_visibility_type visibility);

void content_status_interactions(struct session* ssn,
                                 mastodont_t* api,
                                 char* label,
                                 struct mstdnt_account* accts,
                                 size_t accts_len);

// Status frontends
void status_view(struct session* ssn, mastodont_t* api, char** data);
void status_reply(struct session* ssn, mastodont_t* api, char** data);
void status_interact(struct session* ssn, mastodont_t* api, char** data);
void status_emoji(struct session* ssn, mastodont_t* api, char** data);
// Above wraps to the below function
void content_status(struct session* ssn, mastodont_t* api, char** data, uint8_t flags);

// Cleanup
void cleanup_media_ids(struct session* ssn, char** media_ids);

// Redirects
void notice_redirect(struct session* ssn, mastodont_t* api, char** data);

// API
void api_status_interact(struct session* ssn, mastodont_t* api, char** data);

#endif // STATUS_H

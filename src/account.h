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

#ifndef ACCOUNT_H
#define ACCOUNT_H
#include "global_perl.h"
#include <stddef.h>
#include <mastodont.h>
#include "session.h"
#include "path.h"
#include "l10n.h"

#define ACCOUNT_NOP 0
#define ACCOUNT_ACTION_BTNS (1<<0)
#define ACCOUNT_SUMMARY (1<<1)

enum account_tab
{
    ACCT_TAB_NONE,
    ACCT_TAB_STATUSES,
    ACCT_TAB_SCROBBLES,
    ACCT_TAB_PINNED,
    ACCT_TAB_MEDIA,
};

struct account_page
{
    enum l10n_locale locale;
    struct mstdnt_account* account;
    char* header_image;
    char* profile_image;
    char* acct;
    char* display_name;
    size_t statuses_count;
    size_t following_count;
    size_t followers_count;
    char* id;
    char* note;
    enum account_tab tab;
    mstdnt_relationship_flag_t flags;
    struct mstdnt_relationship* relationship;
};

void get_account_info(mastodont_t* api, struct session* ssn);
char* construct_account_sidebar(struct mstdnt_account* acct, size_t* size);

char* construct_account(mastodont_t* api,
                        struct mstdnt_account* account,
                        uint8_t flags,
                        size_t* size);
char* construct_accounts(mastodont_t* api,
                         struct mstdnt_account* accounts,
                         size_t size,
                         uint8_t flags,
                         size_t* ret_size);

size_t construct_account_page(struct session *ssn,
                              char** result,
                              struct account_page* page,
                              char* content);

char* load_account_page(struct session* ssn,
                        mastodont_t* api,
                        struct mstdnt_account* acct,
                        struct mstdnt_relationship* relationship,
                        enum account_tab tab,
                        char* content,
                        size_t* res_size);

char* load_account_info(struct mstdnt_account* acct,
                        size_t* size);

void content_account_followers(PATH_ARGS);
void content_account_following(PATH_ARGS);
void content_account_statuses(PATH_ARGS);
void content_account_scrobbles(PATH_ARGS);
void content_account_pinned(PATH_ARGS);
void content_account_blocked(PATH_ARGS);
void content_account_muted(PATH_ARGS);
void content_account_media(PATH_ARGS);
void content_account_action(PATH_ARGS);
void content_account_favourites(PATH_ARGS);
void content_account_bookmarks(PATH_ARGS);

HV* perlify_account(const struct mstdnt_account* acct);
AV* perlify_accounts(const struct mstdnt_account* accounts, size_t len);
HV* perlify_scrobble(const struct mstdnt_scrobble* scrobble);
AV* perlify_scrobbles(const struct mstdnt_scrobble* scrobbles, size_t len);
HV* perlify_relationship(const struct mstdnt_relationship* rel);

#endif // ACCOUNT_H

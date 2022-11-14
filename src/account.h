/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
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

int content_account_followers(PATH_ARGS);
int content_account_following(PATH_ARGS);
int content_account_statuses(PATH_ARGS);
int content_account_scrobbles(PATH_ARGS);
int content_account_pinned(PATH_ARGS);
int content_account_blocked(PATH_ARGS);
int content_account_muted(PATH_ARGS);
int content_account_media(PATH_ARGS);
int content_account_action(PATH_ARGS);
int content_account_favourites(PATH_ARGS);
int content_account_bookmarks(PATH_ARGS);

HV* perlify_account(const struct mstdnt_account* acct);
AV* perlify_accounts(const struct mstdnt_account* accounts, size_t len);
HV* perlify_relationship(const struct mstdnt_relationship* rel);

#endif // ACCOUNT_H

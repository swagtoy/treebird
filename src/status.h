/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef STATUS_H
#define STATUS_H
#include <stdint.h>
#include <mastodont.h>
#include "l10n.h"
#include "path.h"
#include "session.h"
#include "global_perl.h"

// Flags
#define STATUS_NOOP 0
#define STATUS_REPLY (1<<0)
#define STATUS_FOCUSED (1<<1)
#define STATUS_EMOJI_PICKER (1<<2)
#define STATUS_NO_LIKEBOOST (1<<3)
#define STATUS_NO_DOPAMEME (1<<4)
#define STATUS_NO_QUICKREPLY (1<<5)

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
int content_status_create(PATH_ARGS);
int content_status_react(PATH_ARGS);

// HTML Builders

// Reply to
#if 0
/** Deprecated: May be used in the future for Mastodon only */
char* get_in_reply_to(mastodont_t* api,
                      struct session* ssn,
                      struct mstdnt_status* status,
                      size_t* size);
#endif

int status_view_reblogs(PATH_ARGS);
int status_view_favourites(PATH_ARGS);

void content_status_interactions(FCGX_Request* req,
                                 struct session* ssn,
                                 mastodont_t* api,
                                 char* label,
                                 struct mstdnt_account* accts,
                                 size_t accts_len);

// Status frontends
int status_view(PATH_ARGS);
int status_reply(PATH_ARGS);
int status_interact(PATH_ARGS);
int status_emoji(PATH_ARGS);
// Above wraps to the below function
void content_status(PATH_ARGS, uint8_t flags);

// Cleanup
void cleanup_media_ids(struct session* ssn, char** media_ids);

// Redirects
int notice_redirect(PATH_ARGS);

// API
int api_status_interact(PATH_ARGS);

// Perl
HV* perlify_status_pleroma(const struct mstdnt_status_pleroma* pleroma);
HV* perlify_status(const struct mstdnt_status* status);
AV* perlify_statuses(const struct mstdnt_status* statuses, size_t len); 

#endif // STATUS_H

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef SESSION_H
#define SESSION_H
#include <mastodont.h>
#include "global_perl.h"
#include "query.h"
#include "local_config.h"
#include "cookie.h"

/* One per connection, contains all the data for a connection
 * as it travels along */
struct session
{
    struct post_values post;
    struct get_values query;
    struct cookie_values cookies;
    struct local_config config;
    int logged_in;
    struct mstdnt_account acct;
    struct mstdnt_storage acct_storage;
    struct mstdnt_args m_args;

    char* cookies_str;
    char* post_str;
    char* get_str;
};

const char* const get_instance(struct session* ssn);
const char* const get_token(struct session* ssn);
HV* perlify_session(struct session* ssn);

void session_cleanup(struct session* ssn);

#endif // SESSION_H

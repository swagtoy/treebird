/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include "page_config.h"
#include "global_perl.h"
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "base_page.h"
#include "../config.h"
#include "easprintf.h"
#include "query.h"
#include "cookie.h"
#include "local_config_set.h"
#include "string_helpers.h"
#include "l10n.h"
#include <fcgi_stdio.h>

#define bool_checked(key) (ssn->config.key ? "checked" : "")

enum config_category
{
    CONFIG_CAT_GENERAL,
    CONFIG_CAT_APPEARANCE,
    CONFIG_CAT_ACCOUNT
};

int content_config_general(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));
    
    PERL_STACK_SCALAR_CALL("config::general");
    
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_CONFIG,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    // Cleanup
    tb_free(dup);
}


int content_config_appearance(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    
    PERL_STACK_SCALAR_CALL("config::appearance");
    
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_CONFIG,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    // Cleanup
    tb_free(dup);
}

int content_config(PATH_ARGS)
{
    redirect(req, REDIRECT_303, "/config/general");
}

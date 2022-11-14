/*
 * Treebird - Lightweight frontend for Pleroma
 * 
 * Licensed under BSD 2-Clause License
 */

#include <stdlib.h>
#include "base_page.h"
#include "about.h"

int content_about(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    mXPUSHs(newRV_inc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));

    PERL_STACK_SCALAR_CALL("meta::about");
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    tb_free(dup);
    return 0;
}


int content_about_license(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));

    PERL_STACK_SCALAR_CALL("meta::license");

    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    tb_free(dup);
    return 0;
}


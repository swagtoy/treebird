/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#include "memory_page.h"
#include "base_page.h"
#include "memory.h"

int content_memory_stats(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    mXPUSHs(newRV_inc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));
    // Args
    mXPUSHi(tb_memory_allocated_est);
    mXPUSHi(tb_memory_allocated);
    mXPUSHi(tb_memory_free);
    mXPUSHi(tb_memory_times_allocated);
    mXPUSHi(tb_memory_times_freed);

    PERL_STACK_SCALAR_CALL("meta::memory");
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    tb_free(dup);
}

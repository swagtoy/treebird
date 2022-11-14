/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <stdlib.h>
#include "global_perl.h"
#include "helpers.h"
#include "base_page.h"
#include "../config.h"
#include "account.h"
#include "easprintf.h"
#include "error.h"
#include "string.h"
#include "status.h"
#include "lists.h"
#include "string_helpers.h"
#include "http.h"

int content_lists(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_list* lists = NULL;
    size_t lists_len = 0;
    struct mstdnt_storage storage = { 0 };

    if (ssn->post.title.is_set)
    {
        struct mstdnt_storage create_storage = { 0 };
        struct mstdnt_list_args args = {
            .title = keystr(ssn->post.title),
            .replies_policy = MSTDNT_LIST_REPLIES_POLICY_LIST,
        };
        mstdnt_create_list(api, &m_args, NULL, NULL, &args, &create_storage, NULL);
        mstdnt_storage_cleanup(&create_storage);
    }

    mstdnt_get_lists(api, &m_args, NULL, NULL, &storage, &lists, &lists_len);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (lists)
        mXPUSHs(newRV_noinc((SV*)perlify_lists(lists, lists_len)));
    
    PERL_STACK_SCALAR_CALL("lists::content_lists");

    // Duplicate so we can free the TMPs
    char* dup = PERL_GET_STACK_EXIT;

    struct base_page b = {
        .category = BASE_CAT_LISTS,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);

    // Cleanup
    mstdnt_storage_cleanup(&storage);
    mstdnt_cleanup_lists(lists);
    tb_free(dup);
}

int list_edit(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    char* referer = getenv("HTTP_REFERER");
    char* id = data[0];

    struct mstdnt_list_args args = {
        .title = keystr(ssn->post.title),
        .replies_policy = keyint(ssn->post.replies_policy)
    };

    mstdnt_update_list(api,
                       &m_args,
                       NULL,
                       NULL,
                       id,
                       &args,
                       &storage,
                       NULL);

    redirect(req, REDIRECT_303, referer);
    mstdnt_storage_cleanup(&storage);
}

HV* perlify_list(const struct mstdnt_list* list)
{
    if (!list) return NULL;

    HV* list_hv = newHV();
    hvstores_str(list_hv, "id", list->id);
    hvstores_str(list_hv, "title", list->title);
//    hvstores_int(list_hv, "replies_policy", list->replies_policy);

    return list_hv;
}

PERLIFY_MULTI(list, lists, mstdnt_list)


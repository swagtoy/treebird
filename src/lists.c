/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <stdlib.h>
#include "global_perl.h"
#include "helpers.h"
#include "base_page.h"
#include "config.h"
#include "account.h"
#include "easprintf.h"
#include "error.h"
#include "string.h"
#include "status.h"
#include "lists.h"
#include "string_helpers.h"
#include "http.h"

static int
request_cb_get_lists(mstdnt_request_cb_data* cb_data,
                     void* args)
{
    struct mstdnt_lists* lists = MSTDNT_CB_DATA(cb_data);
    struct path_args_data* path_data = args;
    
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(path_data->ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (lists)
        mXPUSHs(newRV_noinc((SV*)perlify_lists(lists->lists, lists->len)));
    
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
    render_base_page(&b, path_data->req, path_data->ssn, path_data->api);

    // Cleanup
    tb_free(dup);
    path_args_data_destroy(args);
    return MSTDNT_REQUEST_DONE;
}

int content_lists(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    struct path_args_data* path_data = path_args_data_create(req,
                                                             ssn,
                                                             api,
                                                             NULL);

    if (ssn->post.title.is_set)
    {
        struct mstdnt_storage create_storage = { 0 };
        struct mstdnt_list_args args = {
            .title = keystr(ssn->post.title),
            .replies_policy = MSTDNT_LIST_REPLIES_POLICY_LIST,
        };
        // TODO 
        mstdnt_create_list(api, &m_args, NULL, NULL, args);
        mstdnt_storage_cleanup(&create_storage);
    }

    mstdnt_get_lists(api, &m_args, request_cb_get_lists, path_data);

    
}

int list_edit(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
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
                       args);

    redirect(req, REDIRECT_303, referer);
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


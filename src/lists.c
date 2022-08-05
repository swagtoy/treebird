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
// Files
#include "../static/account.ctmpl"
#include "../static/list.ctmpl"
#include "../static/lists.ctmpl"

void content_lists(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_list* lists = NULL;
    size_t list_len = 0;
    struct mstdnt_storage storage = { 0 };

    if (ssn->post.title.is_set)
    {
        struct mstdnt_storage create_storage = { 0 };
        struct mstdnt_list_args args = {
            .title = keystr(ssn->post.title),
            .replies_policy = MSTDNT_LIST_REPLIES_POLICY_LIST,
        };
        mastodont_create_list(api, &m_args, &args, &create_storage, NULL);
        mastodont_storage_cleanup(&create_storage);
    }

    mastodont_get_lists(api, &m_args, &storage, &lists, &list_len);

    // Call perl
    perl_lock();
    dSP;
    ENTER;
    SAVETMPS;
    PUSHMARK(SP);
    
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    // TODO Perlify lists
    
    // ARGS
    PUTBACK;
    call_pv("lists::content_lists", G_SCALAR);
    SPAGAIN;

    // Duplicate so we can free the TMPs
    char* dup = savesharedsvpv(POPs);
    PUTBACK;
    FREETMPS;
    LEAVE;
    perl_unlock();

    struct base_page b = {
        .category = BASE_CAT_LISTS,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_lists(lists);
    Safefree(dup);
}

void list_edit(PATH_ARGS)
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

    mastodont_update_list(api,
                          &m_args,
                          id,
                          &args,
                          &storage,
                          NULL);

    redirect(req, REDIRECT_303, referer);
    mastodont_storage_cleanup(&storage);
}

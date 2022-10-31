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
#include "search.h"
#include "http.h"
#include "helpers.h"
#include "easprintf.h"
#include "../config.h"
#include "string_helpers.h"
#include "base_page.h"
#include "status.h"
#include "hashtag.h"
#include "error.h"
#include "account.h"

int content_search_all(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_search_args args = {
        .account_id = NULL,
        .type = 0,
        .resolve = 0,
        .following = 0,
        .with_relationships = 0,
        .max_id = NULL,
        .min_id = NULL,
        .since_id = NULL,
        .offset = 0,
        .limit = 20,
    };
    struct mstdnt_search_results results = { 0 };

    // Perform redirect to correct direct page
    if (keyint(ssn->query.type))
    {
        char* query = keystr(ssn->query.query);
        query = curl_easy_escape(api->curl, query, 0);
        char* url;
        // Note: This can be zero, which is just "nothing"
        switch (keyint(ssn->query.type))
        {
        case 1:
            easprintf(&url, "/search/statuses?q=%s", query);
            redirect(req, REDIRECT_303, url);
            break;
        case 2:
            easprintf(&url, "/search/accounts?q=%s", query);
            redirect(req, REDIRECT_303, url);
            break;
        case 3:
            easprintf(&url, "/search/hashtags?q=%s", query);
            redirect(req, REDIRECT_303, url);
            break;
        }
        tb_free(url);
        curl_free(query);
        return;
    }

    mstdnt_search(api, &m_args, NULL, NULL, keystr(ssn->query.query), &storage, &args, &results);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    mXPUSHs(newRV_noinc((SV*)perlify_search_results(&results)));
    
    PERL_STACK_SCALAR_CALL("search::content_search");

    // Duplicate so we can free the TMPs
    char* dup = PERL_GET_STACK_EXIT;

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);

    mstdnt_cleanup_search_results(&results);
    mstdnt_storage_cleanup(&storage);
    tb_free(dup);
}

int content_search_statuses(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* statuses_html;
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_search_args args = {
        .account_id = NULL,
        .type = MSTDNT_SEARCH_STATUSES,
        .resolve = 0,
        .following = 0,
        .with_relationships = 0,
        .max_id = NULL,
        .min_id = NULL,
        .since_id = NULL,
        .offset = 0,
        .limit = 20,
    };
    struct mstdnt_search_results results = { 0 };

    mstdnt_search(api, &m_args, NULL, NULL, keystr(ssn->query.query), &storage, &args, &results);

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    mXPUSHs(newRV_noinc((SV*)perlify_search_results(&results)));
    
    PERL_STACK_SCALAR_CALL("search::content_search_statuses");

    // Duplicate so we can free the TMPs
    char* dup = PERL_GET_STACK_EXIT;

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);

    mstdnt_cleanup_search_results(&results);
    mstdnt_storage_cleanup(&storage);
    tb_free(dup);
}

int content_search_accounts(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_search_args args = {
        .account_id = NULL,
        .type = MSTDNT_SEARCH_ACCOUNTS,
        .resolve = 0,
        .following = 0,
        .with_relationships = 0,
        .max_id = NULL,
        .min_id = NULL,
        .since_id = NULL,
        .offset = 0,
        .limit = 20,
    };
    struct mstdnt_search_results results = { 0 };

    mstdnt_search(api, &m_args, NULL, NULL, keystr(ssn->query.query), &storage, &args, &results);
    
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    mXPUSHs(newRV_noinc((SV*)perlify_search_results(&results)));
    
    PERL_STACK_SCALAR_CALL("search::content_search_accounts");

    // Duplicate so we can free the TMPs
    char* dup = PERL_GET_STACK_EXIT;

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    
    mstdnt_cleanup_search_results(&results);
    mstdnt_storage_cleanup(&storage);
    tb_free(dup);
}

int content_search_hashtags(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_search_args args = {
        .account_id = NULL,
        .type = MSTDNT_SEARCH_HASHTAGS,
        .resolve = 0,
        .following = 0,
        .with_relationships = 0,
        .max_id = NULL,
        .min_id = NULL,
        .since_id = NULL,
        .offset = 0,
        .limit = 20,
    };
    struct mstdnt_search_results results = { 0 };

    mstdnt_search(api, &m_args, NULL, NULL, keystr(ssn->query.query), &storage, &args, &results);
    
    // TODO
    
    mstdnt_cleanup_search_results(&results);
    mstdnt_storage_cleanup(&storage);
//    tb_free(dup);
}

HV* perlify_search_results(struct mstdnt_search_results* results)
{
    if (!results) return NULL;
    
    HV* search_hv = newHV();
    hvstores_ref(search_hv, "accounts", perlify_accounts(results->accts, results->accts_len));
    hvstores_ref(search_hv, "statuses", perlify_statuses(results->statuses, results->statuses_len));
    // TODO tags

    return search_hv;
}

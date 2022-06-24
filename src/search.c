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
#include "easprintf.h"
#include "../config.h"
#include "string_helpers.h"
#include "base_page.h"
#include "status.h"
#include "hashtag.h"
#include "error.h"
#include "account.h"
#include "graphsnbars.h"

// Pages
#include "../static/search.ctmpl"
#include "../static/search_all.ctmpl"

void search_page(struct session* ssn, mastodont_t* api, enum search_tab tab, char* content)
{
    char* out_data;
    struct search_template tdata = {
        .prefix = config_url_prefix,
        .query = keystr(ssn->query.query),
        .accounts_active = MAKE_FOCUSED_IF(tab, SEARCH_ACCOUNTS),
        .accounts = "Accounts",
        .hashtags_active = MAKE_FOCUSED_IF(tab, SEARCH_HASHTAGS),
        .hashtags = "Hashtags",
        .statuses_active = MAKE_FOCUSED_IF(tab, SEARCH_STATUSES),
        .statuses = "Statuses",
        .results = content
    };
    out_data = tmpl_gen_search(&tdata, NULL);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = out_data,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);
    free(out_data);
}

void content_search_all(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* out_data = NULL;
    char* statuses_html = NULL;
    char* accounts_html = NULL;
    char* tags_html = NULL,
        * tags_graph = NULL,
        * tags_bars = NULL,
        * tags_page = NULL;
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

    if (mastodont_search(api,
                         &m_args,
                         keystr(ssn->query.query),
                         &storage,
                         &args,
                         &results) == 0)
    {
        // Statuses, make sure to set the highlight word
        struct construct_statuses_args statuses_args = {
            .highlight_word = keystr(ssn->query.query),
        };
        
        statuses_html = construct_statuses(ssn, api, results.statuses, results.statuses_len, &statuses_args, NULL);
        if (!statuses_html)
            statuses_html = construct_error("No statuses", E_ERROR, 1, NULL);

        // Accounts
        accounts_html = construct_accounts(api, results.accts, results.accts_len, 0, NULL);
        if (!accounts_html)
            accounts_html = construct_error("No accounts", E_ERROR, 1, NULL);

        // Hashtags
        tags_html = construct_hashtags(results.tags, results.tags_len, NULL);
        if (!tags_html)
            tags_html = construct_error("No hashtags", E_ERROR, 1, NULL);

        tags_bars = construct_hashtags_graph(results.tags,
                                             results.tags_len,
                                             14,
                                             NULL);
        if (tags_bars)
            tags_graph = construct_bar_graph_container(tags_bars, NULL);
        
        free(tags_bars);
    }

    easprintf(&tags_page, "%s%s", STR_NULL_EMPTY(tags_graph), tags_html);
    
    // Construct search page
    struct search_all_template tdata = {
        .accounts = "Accounts",
        .hashtags = "Hashtags",
        .statuses = "Statuses",
        .statuses_results = statuses_html,
        .hashtags_results = tags_page,
        .accounts_results = accounts_html
    };
    out_data = tmpl_gen_search_all(&tdata, NULL);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = out_data,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);
    free(out_data);    
    free(statuses_html);
    free(accounts_html);
    free(tags_html);
    free(tags_graph);
    free(tags_page);
    mstdnt_cleanup_search_results(&results);
    mastodont_storage_cleanup(&storage);
}

void content_search_statuses(struct session* ssn, mastodont_t* api, char** data)
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

    if (mastodont_search(api,
                         &m_args,
                         keystr(ssn->query.query),
                         &storage,
                         &args,
                         &results) == 0)
    {
        struct construct_statuses_args statuses_args = {
            .highlight_word = keystr(ssn->query.query),
        };
        statuses_html = construct_statuses(ssn, api, results.statuses, results.statuses_len, &statuses_args, NULL);
        if (!statuses_html)
            statuses_html = construct_error("No statuses", E_ERROR, 1, NULL);
    }
    else
        statuses_html = construct_error("An error occured.", E_ERROR, 1, NULL);
    
    search_page(ssn, api, SEARCH_STATUSES, STR_NULL_EMPTY(statuses_html));
    
    if (statuses_html) free(statuses_html);
    mstdnt_cleanup_search_results(&results);
    mastodont_storage_cleanup(&storage);
}

void content_search_accounts(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* accounts_html;
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

    if (mastodont_search(api,
                         &m_args,
                         keystr(ssn->query.query),
                         &storage,
                         &args,
                         &results) == 0)
    {
        accounts_html = construct_accounts(api, results.accts, results.accts_len, 0, NULL);
        if (!accounts_html)
            accounts_html = construct_error("No accounts", E_ERROR, 1, NULL);
    }
    else
        accounts_html = construct_error("An error occured.", E_ERROR, 1, NULL);
    
    search_page(ssn, api, SEARCH_ACCOUNTS, STR_NULL_EMPTY(accounts_html));
    
    if (accounts_html) free(accounts_html);
    mstdnt_cleanup_search_results(&results);
    mastodont_storage_cleanup(&storage);
}

void content_search_hashtags(struct session* ssn, mastodont_t* api, char** data)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    char* tags_html;
    char* tags_graph = NULL;
    char* tags_bars = NULL;
    char* tags_page;
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

    if (mastodont_search(api,
                         &m_args,
                         keystr(ssn->query.query),
                         &storage,
                         &args,
                         &results) == 0)
    {
        tags_html = construct_hashtags(results.tags, results.tags_len, NULL);
        if (!tags_html)
            tags_html = construct_error("No hashtags", E_ERROR, 1, NULL);

        tags_bars = construct_hashtags_graph(results.tags,
                                             results.tags_len,
                                             14,
                                             NULL);
        if (tags_bars)
            tags_graph = construct_bar_graph_container(tags_bars, NULL);
        
        if (tags_bars) free(tags_bars);
    }
    else
        tags_html = construct_error("An error occured.", E_ERROR, 1, NULL);

    easprintf(&tags_page, "%s%s", STR_NULL_EMPTY(tags_graph), tags_html);
    
    search_page(ssn, api, SEARCH_HASHTAGS, tags_page);
    
    if (tags_html) free(tags_html);
    if (tags_graph) free(tags_graph);
    free(tags_page);
    mstdnt_cleanup_search_results(&results);
    mastodont_storage_cleanup(&storage);
}

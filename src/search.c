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
#include "error.h"

// Pages
#include "../static/search.chtml"

void search_page(struct session* ssn, mastodont_t* api, enum search_tab tab, char* content)
{
    char* out_data;
    easprintf(&out_data, data_search_html,
              config_url_prefix,
              ssn->query.query,
              MAKE_FOCUSED_IF(tab, SEARCH_STATUSES),
              "Statuses",
              config_url_prefix,
              ssn->query.query,
              MAKE_FOCUSED_IF(tab, SEARCH_ACCOUNTS),
              "Accounts",
              config_url_prefix,
              ssn->query.query,
              MAKE_FOCUSED_IF(tab, SEARCH_HASHTAGS),
              "Hashtags",
              content);

    struct base_page b = {
        .category = BASE_CAT_NONE,
        .locale = L10N_EN_US,
        .content = out_data,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    free(out_data);    
}

void content_search_statuses(struct session* ssn, mastodont_t* api, char** data)
{
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
                         ssn->query.query,
                         &storage,
                         &args,
                         &results) == 0)
    {
        struct construct_statuses_args statuses_args = {
            .highlight_word = ssn->query.query,
        };
        statuses_html = construct_statuses(api, results.statuses, results.statuses_len, &statuses_args, NULL);
        if (!statuses_html)
            statuses_html = construct_error("No statuses", E_ERROR, 1, NULL);
    }
    else
        statuses_html = construct_error("An error occured.", E_ERROR, 1, NULL);
    
    search_page(ssn, api, SEARCH_STATUSES, STR_NULL_EMPTY(statuses_html));
    
    if (statuses_html) free(statuses_html);
}

void content_search_accounts(struct session* ssn, mastodont_t* api, char** data)
{
    char* accounts_html;
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
                         ssn->query.query,
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
}

void content_search_hashtags(struct session* ssn, mastodont_t* api, char** data)
{
    search_page(ssn, api, SEARCH_HASHTAGS, "hashtags");
}

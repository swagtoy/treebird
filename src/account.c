/*
 * RatFE - Lightweight frontend for Pleroma
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
#include <stdio.h>
#include "base_page.h"
#include "../config.h"
#include "account.h"
#include "easprintf.h"
#include "status.h"

// Files
#include "../static/index.chtml"
#include "../static/account.chtml"

char* construct_account_page(struct mstdnt_account* acct,
                             struct mstdnt_status* statuses,
                             size_t statuses_len,
                             size_t* res_size)
{
    int cleanup = 0;
    int result_size;
    char* statuses_html;
    char* result;

    // Load statuses html
    statuses_html = construct_statuses(statuses, statuses_len, NULL);
    if (!statuses_html)
        statuses_html = "Error in malloc!";
    else
        cleanup = 1;
    
    result_size = easprintf(&result, data_account_html,
                            acct->header,
                            acct->display_name,
                            acct->acct,
                            acct->avatar,
                            "Statuses",
                            acct->statuses_count,
                            "Following",
                            acct->following_count,
                            "Followers",
                            acct->followers_count,
                            statuses_html);
    
    if (result_size == -1)
        result = NULL;

    if (res_size) *res_size = result_size;
    if (cleanup) free(statuses_html);
    return result;
}

void content_account(mastodont_t* api, char** data, size_t size)
{
    int cleanup = 0;
    char* account_page;
    struct mstdnt_account acct;
    struct mstdnt_storage storage, status_storage;
    struct mstdnt_status* statuses;
    size_t status_len;
    int lookup_type = config_experimental_lookup ? MSTDNT_LOOKUP_ACCT : MSTDNT_LOOKUP_ID;

    if (mastodont_account(api, lookup_type, data[0],
                          &acct, &storage, NULL) ||
        mastodont_account_statuses(api, acct.id, NULL,
                                   &status_storage, &statuses, &status_len))
    {
        account_page = "Couldn't load account info";
    }
    else {
        cleanup = 1;
        account_page = construct_account_page(&acct,
                                              statuses,
                                              status_len,
                                              NULL);
        if (!account_page)
            account_page = "Malloc error";
    }
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = account_page,
        .sidebar_right = NULL
    };

    /* Output */
    render_base_page(&b);

    /* Cleanup */
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&status_storage);
    if (cleanup) free(account_page);
}

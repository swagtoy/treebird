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

// Files
#include "../static/index.chtml"
#include "../static/account.chtml"

char* construct_account_page(struct mstdnt_account* acct, size_t* res_size)
{
    int result_size;
    char* result;
    result_size = easprintf(&result, data_account_html,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL);
    if (result_size == -1)
        result = NULL;

    if (res_size) *res_size = result_size;
    return result;
}

void content_account(mastodont_t* api)
{
    char* account_page;
    account_page = construct_account_page(NULL, NULL);
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = account_page,
        .sidebar_right = NULL
    };

    /* Output */
    render_base_page(&b);
}

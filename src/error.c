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
#include "base_page.h"
#include "error.h"
#include "easprintf.h"
#include "l10n.h"

// Pages
#include "../static/error_404.chtml"
#include "../static/error.chtml"

char* construct_error(char* error, size_t* size)
{
    char* error_html;
    size_t s = easprintf(&error_html, data_error_html,
                         error);
    if (size) *size = s;
    return error_html;
}

void content_not_found(struct session* ssn, mastodont_t* api, char* path)
{
    char* page;
    easprintf(&page,
              data_error_404_html,
              L10N[L10N_EN_US][L10N_PAGE_NOT_FOUND]);
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = page,
        .sidebar_left = NULL
    };

    render_base_page(&b, ssn, api);
    free(page);
}

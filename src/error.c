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
#include "../static/error_404.ctmpl"
#include "../static/error.ctmpl"

char* construct_error(const char* error, enum error_type type, unsigned pad, size_t* size)
{
    char* class;

    switch (type)
    {
    case E_ERROR:
        class = "error"; break;
    case E_WARNING:
        class = "warning"; break;
    case E_NOTICE:
        class = "notice"; break;
    }

    struct error_template data = {
        .err_type = class,
        .is_padded = pad ? "error-pad" : NULL,
        .error = error ? error : "An error occured",
    };
    
    return tmpl_gen_error(&data, size);
}

void content_not_found(struct session* ssn, mastodont_t* api, char* path)
{
    char* page;
    struct error_404_template data = {
        .error = L10N[L10N_EN_US][L10N_PAGE_NOT_FOUND]
    };
    page = tmpl_gen_error_404(&data, NULL);
    
    struct base_page b = {
        .content = page,
        .sidebar_left = NULL
    };

    render_base_page(&b, ssn, api);
    free(page);
}

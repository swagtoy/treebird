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

// Pages
#include "../static/search.chtml"

void content_search(struct session* ssn, mastodont_t* api, char** data)
{
    char* out_data;
    easprintf(&out_data, data_search_html,
              config_url_prefix,
              ssn->query.query,
              "Statuses",
              config_url_prefix,
              ssn->query.query,
              "Accounts",
              config_url_prefix,
              ssn->query.query,
              "Hashtags",
              "Nothing");

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

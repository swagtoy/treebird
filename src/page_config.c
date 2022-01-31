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

#include <stdio.h>
#include "../config.h"
#include "page_config.h"

// Pages
#include "../static/index.chtml"
#include "../static/config.chtml"

void content_config(mastodont_t* api)
{
    (void)api; // No need to use this
    
    /* Output */
    printf("Content-Length: %ld\r\n\r\n",
           data_index_html_size + data_config_html_size);
    printf(data_index_html, config_canonical_name, data_config_html);
}

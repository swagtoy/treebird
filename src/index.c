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
#include "../config.h"
#include "index.h"
#include "easprintf.h"
#include "status.h"

// Files
#include "../static/index.chtml"

void content_index(mastodont_t* api)
{
    size_t status_count, statuses_html_count;
    struct mstdnt_status* statuses;
    struct mstdnt_storage storage;
    char* status_format;
    mastodont_timeline_public(api, NULL, &storage, &statuses, &status_count);

    /* Construct statuses into HTML */
    status_format = construct_statuses(statuses, status_count, &statuses_html_count);
    if (status_format == NULL)
        status_format = "Error in malloc!";

    /* Output */
    printf("Content-Length: %ld\r\n\r\n",
           data_index_html_size + statuses_html_count + 1);
    printf(data_index_html, config_canonical_name, status_format);

    /* Cleanup */
    mastodont_storage_cleanup(&storage);
    free(status_format);
}

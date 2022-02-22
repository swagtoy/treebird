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
#include "index.h"
#include "status.h"
#include "easprintf.h"

// Files
#include "../static/index.chtml"
#include "../static/post.chtml"

void content_index(mastodont_t* api)
{
    int cleanup = 0;
    size_t status_count, statuses_html_count;
    struct mstdnt_status* statuses;
    struct mstdnt_storage storage;
    char* status_format;
    char* output = NULL;
    
    try_post_status(api);
    try_interact_status(api);
    
    if (mastodont_timeline_public(api, NULL, &storage, &statuses, &status_count))
    {
        status_format = "An error occured loading the timeline";
    }
    else
    {
        /* Construct statuses into HTML */
        status_format = construct_statuses(statuses, status_count, &statuses_html_count);
        if (!status_format)
            status_format = "Error in malloc!";
        cleanup = 1;
    }

    easprintf(&output, "%s %s", data_post_html, status_format);

    struct base_page b = {
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_right = NULL
    };

    /* Output */
    render_base_page(&b);

    /* Cleanup */
    mastodont_storage_cleanup(&storage);
    if (cleanup) free(status_format);
    if (output) free(output);
}

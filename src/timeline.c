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

#include "timeline.h"
#include <stdlib.h>
#include "base_page.h"
#include "../config.h"
#include "index.h"
#include "status.h"
#include "easprintf.h"
#include "reply.h"
#include "navigation.h"
#include "query.h"
#include "error.h"
#include "string_helpers.h"

#include "../static/navigation.chtml"

void tl_public(struct session* ssn, mastodont_t* api, int local)
{
    size_t status_count, statuses_html_count;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };
    char* status_format = NULL,
        *post_box,
        *navigation_box = NULL,
        *output = NULL;
    char* start_id;

    struct mstdnt_args args = {
        .local = local,
        .remote = 0,
        .only_media = 0,
        .max_id = ssn->post.max_id,
        .since_id = NULL,
        .min_id = ssn->post.min_id,
        .limit = 20
    };

    try_post_status(ssn, api);
    
    if (mastodont_timeline_public(api, &args, &storage, &statuses, &status_count))
    {
        status_format = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        // Construct statuses into HTML
        status_format = construct_statuses(api, statuses, status_count, &statuses_html_count);
        if (!status_format)
            status_format = "Error in malloc!";
    }

    // Create post box
    post_box = construct_post_box(NULL, "", NULL);
    if (statuses)
    {
        // If not set, set it
        start_id = ssn->post.start_id ? ssn->post.start_id : statuses[0].id;
        navigation_box = construct_navigation_box(start_id,
                                                  statuses[0].id,
                                                  statuses[status_count-1].id,
                                                  NULL);
    }
    easprintf(&output, "%s%s%s",
              post_box,
              STR_NULL_EMPTY(status_format),
              STR_NULL_EMPTY(navigation_box));

    struct base_page b = {
        .category = BASE_CAT_HOME,
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_statuses(statuses, status_count);
    if (status_format) free(status_format);
    if (post_box) free(post_box);
    if (navigation_box) free(navigation_box);
    if (output) free(output);
}

void tl_list(struct session* ssn, mastodont_t* api, char* list_id)
{
    size_t status_count, statuses_html_count;
    struct mstdnt_status* statuses = NULL;
    struct mstdnt_storage storage = { 0 };
    char* status_format, *post_box;
    char* output = NULL;

    struct mstdnt_args args = {
        .max_id = NULL,
        .since_id = NULL,
        .min_id = NULL,
        .limit = 20,
    };

    try_post_status(ssn, api);
    
    if (mastodont_timeline_list(api, list_id, &args, &storage, &statuses, &status_count))
    {
        status_format = construct_error(storage.error, E_ERROR, 1, NULL);
    }
    else {
        // Construct statuses into HTML
        status_format = construct_statuses(api, statuses, status_count, &statuses_html_count);
        if (!status_format)
            status_format = construct_error("No statuses", E_ERROR, 1, NULL);
    }

    // Create post box
    post_box = construct_post_box(NULL, "", NULL);
    easprintf(&output, "%s%s%s", post_box,
              status_format,
              data_navigation_html);

    struct base_page b = {
        .category = BASE_CAT_LISTS,
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    mstdnt_cleanup_statuses(statuses, status_count);
    free(status_format);
    if (post_box) free(post_box);
    if (output) free(output);
}


void content_tl_federated(struct session* ssn, mastodont_t* api, char** data)
{
    (void)data;
    tl_public(ssn, api, 0);
}

void content_tl_local(struct session* ssn, mastodont_t* api, char** data)
{
    (void)data;
    tl_public(ssn, api, 1);
}

void content_tl_list(struct session* ssn, mastodont_t* api, char** data)
{
    tl_list(ssn, api, data[0]);
}

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

#include <string.h>
#include "navigation.h"
#include "easprintf.h"

// Pages
#include "../static/navigation.chtml"

#define SUBMIT_HTML "<input type=\"submit\" class=\"hidden\">"

char* construct_navigation_box(char* start_id,
                               char* prev_id,
                               char* next_id,
                               size_t* size)
{
    char* nav_html;

    int is_start = start_id && prev_id ? strcmp(start_id, prev_id) == 0 : 0;

    size_t s = easprintf(&nav_html, data_navigation_html,
                         start_id,
                         prev_id,
                         // Disable button if at start
                         is_start ? "btn-disabled" : "",
                         is_start ? "" : SUBMIT_HTML,
                         // If user pressed next, reserve start state
                         start_id,
                         next_id);
    if (size) *size = s;
    return nav_html;
}

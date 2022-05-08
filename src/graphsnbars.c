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
#include <time.h>
#include "graphsnbars.h"
#include "easprintf.h"
#include "string_helpers.h"

// Pages
#include "../static/bar.chtml"
#include "../static/bar_graph.chtml"

struct hashtags_graph_args
{
    struct mstdnt_tag* tags;
    size_t tags_len;
    unsigned max;
    time_t rel_day;
    size_t days;
};

char* construct_bar_graph_container(char* bars, size_t* size)
{
    char* bar_graph_html;

    size_t s = easprintf(&bar_graph_html, data_bar_graph_html, bars);
    
    if (size) *size = s;
    return bar_graph_html;
}

char* construct_bar(float value, int* size)
{
    char* bar_html;

    size_t s = easprintf(&bar_html, data_bar_html,
                         value*100);

    if (size) *size = s;
    return bar_html;
}

static char* construct_hashgraph_voidwrap(void* passed, size_t index, int* res)
{
    unsigned curr_sum = 0;
    struct hashtags_graph_args* args = passed;
    struct mstdnt_tag* tags = args->tags;
    size_t tags_len = args->tags_len;
    unsigned max = args->max;
    time_t rel_day = args->rel_day;
    size_t days = args->days;

    for (int i = 0; i < tags_len; ++i)
    {
        for (int j = 0; j < tags[i].history_len; ++j)
        {
            if (tags[i].history[j].day == rel_day-((days-index-1)*86400))
                curr_sum += tags[i].history[j].uses;
        }
    }
    
    return construct_bar((float)curr_sum / max, res);
}

char* construct_hashtags_graph(struct mstdnt_tag* tags,
                               size_t tags_len,
                               size_t days,
                               size_t* ret_size)
{
    unsigned max_sum = 0;
    unsigned curr_sum = 0;
    size_t max_history_len = 0;

    // Get current time at midnight for basis, copy over
    time_t t = time(NULL);
    struct tm* mn_ptr = gmtime(&t);
    struct tm mn;
    memcpy(&mn, mn_ptr, sizeof(mn));
    mn.tm_hour = 0;
    mn.tm_min = 0;
    mn.tm_sec = 0;
    time_t rel_day = timegm(&mn);

    // Run a loop through all the hashtags, sum each set up,
    // then get the largest sum
    for (size_t i = 0; i < days; ++i)
    {
        for (size_t j = 0; j < tags_len && i < tags[j].history_len; ++j)
        {
            if (tags[j].history_len > max_history_len)
                max_history_len = tags[j].history_len;
            if (tags[j].history[i].day >= rel_day-(i*86400))
                curr_sum += tags[j].history[i].uses;
        }

        if (curr_sum > max_sum)
            max_sum = curr_sum;
        curr_sum = 0;
    }
    
    struct hashtags_graph_args args = {
        .tags = tags,
        .tags_len = tags_len,
        .max = max_sum,
        .rel_day = rel_day,
        .days = max_history_len,
    };

    return construct_func_strings(construct_hashgraph_voidwrap, &args, max_history_len, ret_size);
}

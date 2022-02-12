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
#include <string.h>
#include <mastodont.h>
#include "../config.h"
#include "index.h"
#include "page_config.h"
#include "path.h"
#include "account.h"
#include "local_config.h"

int main(void)
{
    // Content type is always HTML
    fputs("Content-type: text/html\r\n", stdout);

    // Global init
    mastodont_global_curl_init();

    // API
    mastodont_t api;
    api.url = config_instance_url;
    mastodont_init(&api);

    // Config defaults
    g_config.theme = "ratfe20";

    /*******************
     *  Path handling  *
     ******************/
    struct path_info paths[] = {
        { "/config", content_config },
        { "/@:", content_account },
    };

    handle_paths(&api, paths, sizeof(paths)/sizeof(paths[0]));

    // Cleanup
    mastodont_free(&api);
    mastodont_global_curl_cleanup();
}

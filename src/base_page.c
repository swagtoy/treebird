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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "base_page.h"
#include "easprintf.h"
#include "cookie.h"


// Files
#include "../static/index.chtml"

void render_base_page(struct base_page* page)
{
    char* cookie = getenv("HTTP_COOKIE");
    enum l10n_locale locale = page->locale;
    char* cookie_read = cookie;
    struct http_cookie_info info = { 0 };
    char* login_string = "<a href=\"login\" id=\"login-header\">Login / Register</a>";

    /*
     * Since getenv() returns a pointer to the env variables,
     * we're going to overwrite that data. It saves us some copying
     * time, since this is /very likely/ the last time we will ever
     * read HTTP_COOKIE
     */
    if (!g_config.changed && cookie)
        while (1)
        {
            cookie_read = parse_cookies(cookie_read, &info);
        
            if (!(info.key && info.val)) break;
            if (strcmp(info.key, "theme") == 0)
            {
                g_config.theme = info.val;
            }
            else if (strcmp(info.key, "logged_in") == 0)
            {
                if (strcmp(info.val, "t") == 0)
                    login_string = "";
            }

            if (!cookie_read) break;
        }
    
    char* data;
    int len = easprintf(&data, data_index_html,
                        L10N[locale][L10N_APP_NAME],
                        g_config.theme,
                        L10N[locale][L10N_APP_NAME],
                        login_string,
                        L10N[locale][L10N_SEARCH_PLACEHOLDER],
                        L10N[locale][L10N_SEARCH_BUTTON],
                        L10N[locale][L10N_HOME],
                        L10N[locale][L10N_LOCAL],
                        L10N[locale][L10N_FEDERATED],
                        L10N[locale][L10N_NOTIFICATIONS],
                        L10N[locale][L10N_LISTS],
                        L10N[locale][L10N_DIRECT],
                        L10N[locale][L10N_CONFIG],
                        page->content);
    
    if (!data)
    {
        perror("malloc");
        return;
    }

    printf("Content-Length: %d\r\n\r\n", len + 1);
    puts(data);

    free(data);
}

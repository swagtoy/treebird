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
#include "../config.h"

// Files
#include "../static/index.chtml"

void render_base_page(struct base_page* page)
{
    char* cookie = getenv("HTTP_COOKIE");
    enum l10n_locale locale = page->locale;
    char* login_string = "<a href=\"login\" id=\"login-header\">Login / Register</a>";

    if (!g_config.changed && cookie)
    {
        if (cookies.theme)
            g_config.theme = cookies.theme;
        if (cookies.logged_in && strcmp(cookies.logged_in, "t") == 0)
            login_string = "";
    }
    
    char* data;
    int len = easprintf(&data, data_index_html,
                        L10N[locale][L10N_APP_NAME],
                        g_config.theme,
                        config_url_prefix,
                        L10N[locale][L10N_APP_NAME],
                        login_string,
                        config_url_prefix,
                        L10N[locale][L10N_SEARCH_PLACEHOLDER],
                        L10N[locale][L10N_SEARCH_BUTTON],
                        config_url_prefix,
                        L10N[locale][L10N_HOME],
                        config_url_prefix,
                        L10N[locale][L10N_LOCAL],
                        config_url_prefix,
                        L10N[locale][L10N_FEDERATED],
                        config_url_prefix,
                        L10N[locale][L10N_NOTIFICATIONS],
                        config_url_prefix,
                        L10N[locale][L10N_LISTS],
                        config_url_prefix,
                        L10N[locale][L10N_DIRECT],
                        config_url_prefix,
                        L10N[locale][L10N_CONFIG],
                        page->content);
    
    if (!data)
    {
        perror("malloc");
        return;
    }
    
    fputs("Content-type: text/html\r\n", stdout);
    printf("Content-Length: %d\r\n\r\n", len + 1);
    puts(data);

    free(data);
}

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

#include "http.h"

#define REDIR_HTML_BEGIN "<!DOCTYPE html>" \
    "<html>" \
    "<head>" \
    "<title>Redirecting</title>" \
    "</head>" \
    "<body>"
#define REDIR_HTML_END "</body>" \
    "</html>"

void redirect(REQUEST_T req, char* status, char* location)
{
    char* loc_str = location ? location : "/";
    
    PRINTF("Status: %s\r\n"
           "Location: %s\r\n\r\n"
           REDIR_HTML_BEGIN "Redirecting to <a href=\"\">%s</a>..." REDIR_HTML_END,
           status,
           loc_str,
           loc_str);
}

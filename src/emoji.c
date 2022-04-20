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

#include <stdlib.h>
#include <string.h>
#include "string.h"
#include "emoji.h"
#include "easprintf.h"

char* emojify(char* content, struct mstdnt_emoji* emos, size_t emos_len)
{
    size_t sc_len;
    char* oldres;
    char* res = content;
    char* emoji_url_str;
    char* coloned;
    for (size_t i = 0; i < emos_len; ++i)
    {
        oldres = res;

        // Add colons around string
        sc_len = strlen(emos[i].shortcode);
        // 3 = \0 and two :
        coloned = malloc(sc_len+3);
        coloned[0] = ':';
        strncpy(coloned + 1, emos[i].shortcode, sc_len);
        coloned[sc_len+1] = ':';
        coloned[sc_len+2] = '\0';

        easprintf(&emoji_url_str, "<img class=\"emoji\" src=\"%s\">", emos[i].url);
        
        res = strrepl(res, coloned, emoji_url_str, STRREPL_ALL);
        if (oldres != content) free(oldres);
        // Cleanup
        free(emoji_url_str);
        free(coloned);
    }
    return res;
}

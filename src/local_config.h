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

#ifndef LOCAL_CONFIG_H
#define LOCAL_CONFIG_H
#include "query.h"

struct local_config
{
    int changed;
    char* logged_in;
    char* theme;
    char* background_url;
    int themeclr;
    int jsactions;
    int jsreply;
    int jslive;
    int js;
    int stat_attachments;
    int stat_greentexts;
    int stat_dope;
    int stat_oneclicksoftware;
    int stat_emoji_likes;
    int stat_hide_muted;
    int instance_show_shoutbox;
    int instance_panel;
};

#endif // LOCAL_CONFIG_H

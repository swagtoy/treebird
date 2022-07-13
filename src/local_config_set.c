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

#include <fcgi_stdio.h>
#include <stdlib.h>
#include "local_config_set.h"

#define post_bool_intp(post) (post->is_set ? keypint(post) : 0)

int set_config_str(struct session* ssn,
                   char** v,
                   char* cookie_name,
                   struct key* post,
                   struct key* cookie,
                   enum config_page page,
                   enum config_page curr_page)
{
    if (page == curr_page)
    {
        if (ssn->post.set.is_set && post->is_set && page == curr_page)
        {
            printf("Set-Cookie: %s=%s; HttpOnly; Path=/; Max-Age=31536000; SameSite=Strict;\r\n",
                   cookie_name, keypstr(post));
        }

        if ((ssn->post.set.is_set && post->is_set) || cookie->is_set)
            *v = post->is_set ? keypstr(post) : keypstr(cookie);
    }
    else
        // Set it to the cookie
        *v = keypstr(cookie);
    return 0;
}

int set_config_int(struct session* ssn,
                   int* v,
                   char* cookie_name,
                   struct key* post,
                   struct key* cookie,
                   enum config_page page,
                   enum config_page curr_page)
{
    if (page == curr_page)
    {
        if (ssn->post.set.is_set && page == curr_page)
        {
            printf("Set-Cookie: %s=%d; HttpOnly; Path=/; Max-Age=31536000; SameSite=Strict;\r\n",
                   cookie_name, post_bool_intp(post));
        } 

        // Checks if boolean option
        if (ssn->post.set.is_set || cookie->is_set)
            *v = ssn->post.set.is_set ? post_bool_intp(post)
                : keypint(cookie);
    }
    else
        *v = keypint(cookie);
    
    return 0;
}

// Shorthand for the arguments passed into functions below
#define LOAD_CFG_SIM(strcookie, varname) ssn, &(ssn->config.varname), (strcookie), &(ssn->post.varname), &(ssn->cookies.varname), page

void load_config(struct session* ssn, mastodont_t* api, enum config_page page)
{
    if (ssn->post.set.is_set)
    {
        struct mstdnt_attachment* attachments = NULL;
        struct mstdnt_storage* storage = NULL;
        if (try_upload_media(&storage, ssn, api, &attachments, NULL) == 0)
        {
            struct key atm = { .type.s = attachments[0].url, .is_set = 1 };
            set_config_str(ssn, &(ssn->config.background_url), "background_url", &(atm), &(ssn->cookies.background_url), page, CONFIG_APPEARANCE);
        }

        if (storage)
            cleanup_media_storages(ssn, storage);
    }
    if (!ssn->post.set.is_set)
        ssn->config.background_url = keystr(ssn->cookies.background_url);
    set_config_str(LOAD_CFG_SIM("theme",                theme), CONFIG_APPEARANCE);
    set_config_int(LOAD_CFG_SIM("themeclr",             themeclr), CONFIG_APPEARANCE);
    set_config_int(LOAD_CFG_SIM("jsactions",            jsactions), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("jsreply",              jsreply), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("jslive",               jslive), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("js",                   js), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("statattachments",      stat_attachments), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("statgreentexts",       stat_greentexts), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("statdope",             stat_dope), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("statoneclicksoftware", stat_oneclicksoftware), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("statemojolikes",       stat_emojo_likes), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("stathidemuted",        stat_hide_muted), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("instanceshowshoutbox", instance_show_shoutbox), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("instancepanel",        instance_panel), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("notifembed",           notif_embed), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("interact_img",         interact_img), CONFIG_GENERAL);
    set_config_int(LOAD_CFG_SIM("lang",                 lang), CONFIG_GENERAL);
}

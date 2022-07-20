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

#ifndef LOCAL_CONFIG_SET_H
#define LOCAL_CONFIG_SET_H
#include <mastodont.h>
#include <fcgi_stdio.h>
#include <fcgiapp.h>
#include "local_config.h"
#include "session.h"
#include "attachments.h"
#include "key.h"

enum config_page
{
    CONFIG_GENERAL,
    CONFIG_APPEARANCE,
};

/**
 * Sets a config string
 *
 * @param ssn The session to modify
 * @param v The (config) character pointer to modify
 * @param cookie_name The cookie name to match
 * @param post The post value to check
 * @param cookie The cookie value to store into
 * @param page The page that this value is expected to be set on.
 * @param curr_page The page to check against
 */
void set_config_str(FCGX_Request* req,
                    struct session* ssn,
                    char** v,
                    char* cookie_name,
                    struct key* post,
                    struct key* cookie,
                    enum config_page page,
                    enum config_page curr_page);

/**
 * Sets a config integer
 *
 * @param ssn The session to modify
 * @param v The (config) integer pointer to modify
 * @param cookie_name The cookie name to match
 * @param post The post value to check
 * @param cookie The cookie value to store into
 * @param page The page that this value is expected to be set on.
 * @param curr_page The page to check against
 */
void set_config_int(FCGX_Request* req,
                    struct session* ssn,
                    int* v,
                    char* cookie_name,
                    struct key* post,
                    struct key* cookie,
                    enum config_page page,
                    enum config_page curr_page);

/**
 * Loads the config and sets the values based on POST or session
 * 
 * @param ssn The session
 * @param api mastodont-c api
 * @param page Page enum, to ensure that config changes on different pages don't effect other cookies
 * @return Storage if files were uploaded, must free. This might change
 */
struct mstdnt_storage* load_config(FCGX_Request* req,
                                   struct session* ssn,
                                   mastodont_t* api,
                                   enum config_page page);

#endif // LOCAL_CONFIG_SET_H

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef LOCAL_CONFIG_SET_H
#define LOCAL_CONFIG_SET_H
#include <mastodont.h>
#include "session.h"
#include "attachments.h"
#include "local_config.h"
#include "key.h"
#include "cgi.h"

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
 * @param api mstdnt-c api
 * @param page Page enum, to ensure that config changes on different pages don't effect other cookies
 * @return Storage if files were uploaded, must free. This might change
 */
void load_config(FCGX_Request* req,
                 struct session* ssn,
                 mastodont_t* api,
                 enum config_page page);

#endif // LOCAL_CONFIG_SET_H

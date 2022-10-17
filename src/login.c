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

#include "login.h"
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "query.h"
#include "base_page.h"
#include "error.h"
#include "easprintf.h"
#include "../config.h"
#include "http.h"
#include <curl/curl.h>
#include "cgi.h"
#include "request.h"

#define LOGIN_SCOPE "read+write+follow+push"

static void apply_access_token(REQUEST_T req, char* token)
{
    PRINTF("Set-Cookie: access_token=%s; Path=/; Max-Age=31536000\r\n", token);
    PUT("Set-Cookie: logged_in=t; Path=/; Max-Age=31536000\r\n");
    // if config_url_prefix is empty, make it root
    redirect(req, REDIRECT_303, config_url_prefix &&
             config_url_prefix[0] != '\0' ? config_url_prefix : "/");
}

void content_login_oauth(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 }, oauth_storage = { 0 };
    struct mstdnt_app app;
    struct mstdnt_oauth_token token;
    const char* orig_url = m_args.url;
    char* redirect_url = getenv("SERVER_NAME");
    char* decode_url = NULL;
    char* urlify_redirect_url = NULL;
    easprintf(&urlify_redirect_url, "http%s://%s/login/oauth",
              config_host_url_insecure ? "" : "s",
              config_host_url ? config_host_url : redirect_url );

    if (keystr(ssn->query.code))
    {
        struct mstdnt_application_args args_token = {
            .grant_type = "authorization_code",
            .client_id = keystr(ssn->cookies.client_id),
            .client_secret = keystr(ssn->cookies.client_secret),
            .redirect_uri = urlify_redirect_url,
            .scope = LOGIN_SCOPE,
            .code = keystr(ssn->query.code),
        };

        if (mstdnt_obtain_oauth_token(api,
                                         &m_args,
                                         &args_token,
                                         &oauth_storage,
                                         &token) == 0)
        {
            apply_access_token(req, token.access_token);
        }
    }
    else if (keystr(ssn->post.instance))
    {
        decode_url = curl_easy_unescape(api->curl, keystr(ssn->post.instance), 0, NULL);
        m_args.url = decode_url;
        
        struct mstdnt_application_args args_app = {
            .client_name = "Treebird",
            .redirect_uris = urlify_redirect_url,
            .scopes = "read+write+follow+push",
            .website = keystr(ssn->post.instance)
        };

        if (mstdnt_register_app(api,
                                   &m_args,
                                   &args_app,
                                   &storage,
                                   &app) == 0)
        {
            char* url;
            char* encode_id = curl_easy_escape(api->curl, app.client_id, 0);
            easprintf(&url, "%s/oauth/authorize?response_type=code&scope=" LOGIN_SCOPE "&client_id=%s&redirect_uri=%s",
                      decode_url, encode_id, urlify_redirect_url);

            // Set cookie and redirect
            PRINTF("Set-Cookie: instance_url=%s; Path=/; Max-Age=3153600\r\n", decode_url);
            PRINTF("Set-Cookie: client_id=%s; Path=/; Max-Age=3153600\r\n", app.client_id);
            PRINTF("Set-Cookie: client_secret=%s; Path=/; Max-Age=3153600\r\n", app.client_secret);
            
            redirect(req, REDIRECT_303, url);
            tb_free(url);
            curl_free(encode_id);
        }
    }

    m_args.url = orig_url;
    
    redirect(req, REDIRECT_303, config_url_prefix &&
             config_url_prefix[0] != '\0' ? config_url_prefix : "/");

    mstdnt_storage_cleanup(&storage);
    mstdnt_storage_cleanup(&oauth_storage);
    if (urlify_redirect_url) tb_free(urlify_redirect_url);
    if (decode_url) curl_free(decode_url);
}

void content_login(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 }, oauth_store = { 0 };
    struct mstdnt_app app;
    struct mstdnt_oauth_token token;
    char* error = NULL;
    char* page;

    if (keystr(ssn->post.username) && keystr(ssn->post.password))
    {
        // Getting the client id/secret
        struct mstdnt_application_args args_app = {
            .client_name = "Treebird",
            .redirect_uris = "http://localhost/",
            .scopes = LOGIN_SCOPE,
            .website = NULL
        };

        // Check if the username contains an @ symbol
        char* address = strstr(keystr(ssn->post.username), "%40");
        // If it fails, we need to restore
        const char* orig_url = m_args.url;
        char* url_link = NULL;
        if (address)
        {
            // Let is viewable as username
            *address = '\0';
            address += sizeof("%40")-1;
            easprintf(&url_link, "https://%s/", address);
            m_args.url = url_link;
        }
        else {
            // Reset to instance url
            m_args.url = config_instance_url;
        }

        if (mstdnt_register_app(api, &m_args, &args_app, &storage, &app) != 0)
        {
//            error = construct_error(oauth_store.error, E_ERROR, 1, NULL);
        }
        else {
            struct mstdnt_application_args args_token = {
                .grant_type = "password",
                .client_id = app.client_id,
                .client_secret = app.client_secret,
                .redirect_uri = NULL,
                .scope = LOGIN_SCOPE,
                .code = NULL,
                .username = keystr(ssn->post.username),
                .password = keystr(ssn->post.password)
            };

            if (mstdnt_obtain_oauth_token(api,
                                             &m_args,
                                             &args_token,
                                             &oauth_store,
                                             &token) != 0 && oauth_store.error)
            {
                //error = construct_error(oauth_store.error, E_ERROR, 1, NULL);
            }
            else {
                if (url_link)
                {
                    PRINTF("Set-Cookie: instance_url=%s; Path=/; Max-Age=31536000\r\n", url_link);
                } else
                    // Clear
                    PUT("Set-Cookie: instance_url=; Path=/; Max-Age=-1\r\n");

                apply_access_token(req, token.access_token);
                tb_free(url_link);
                return;
            }
        }
        
        if (url_link)
        {
            // Restore and cleanup, an error occured
            m_args.url = orig_url;
            tb_free(url_link);
        }
    }

    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    if (storage.error || oauth_store.error)
        mXPUSHs(newSVpv(storage.error ? storage.error : oauth_store.error, 0));

    PERL_STACK_SCALAR_CALL("login::content_login");

    page = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = page,
        .session = session_hv,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);

    // Cleanup
    mstdnt_storage_cleanup(&storage);
    mstdnt_storage_cleanup(&oauth_store);
    tb_free(page);
}

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include "login.h"
#include <string.h>
#include <stdlib.h>
#include "fcgiapp.h"
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

static void
apply_access_token(REQUEST_T req, char* token)
{
    PRINTF("Set-Cookie: access_token=%s; Path=/; Max-Age=31536000\r\n", token);
    PUT("Set-Cookie: logged_in=t; Path=/; Max-Age=31536000\r\n");
    // if config_url_prefix is empty, make it root
    redirect(req, REDIRECT_303, config_url_prefix &&
             config_url_prefix[0] != '\0' ? config_url_prefix : "/");
}

int
content_login_oauth(PATH_ARGS)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    
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
                                      NULL, NULL,
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
                                NULL,
                                NULL,
                                args_app) == 0)
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

static int
request_cb_oauth_token(struct mstdnt_request_cb_data* cb_data,
                       void* args)
{
    struct mstdnt_oauth_token* token = MSTDNT_CB_DATA(cb_data);
    struct path_args_data* path_data = args;

    char const* url_link = path_data->ssn->m_args.url;

    // Needed for PRINTF statements, will probably get removed later
    FCGX_Request* req = path_data->req;
    if (url_link)
    {
        PRINTF("Set-Cookie: instance_url=%s; Path=/; Max-Age=31536000\r\n", url_link);
    }
    else {
        // Clears the cookie
        PUT("Set-Cookie: instance_url=; Path=/; Max-Age=-1\r\n");
    }

    apply_access_token(req, token->access_token);
    
    tb_free(url_link);

    path_args_data_destroy(path_data);
    return MSTDNT_REQUEST_DONE;
}

// Callback: mstdnt_register_app
static int
request_cb_register_app(struct mstdnt_request_cb_data* cb_data,
                        void* args)
{
    struct mstdnt_app* app = MSTDNT_CB_DATA(cb_data);
    struct path_args_data* path_data = args;

    mstdnt_obtain_oauth_token(path_data->api,
                              &path_data->ssn->m_args,
                              request_cb_oauth_token,
                              path_data,
                              (struct mstdnt_application_args)
                              {
                                  .grant_type = "password",
                                  .client_id = app->client_id,
                                  .client_secret = app->client_secret,
                                  .redirect_uri = NULL,
                                  .scope = LOGIN_SCOPE,
                                  .code = NULL,
                                  .username = keystr(path_data->ssn->post.username),
                                  .password = keystr(path_data->ssn->post.password)
                              });
    
    return MSTDNT_REQUEST_DONE;
}

// Registers an app, then proceeds to login
static int
register_app(PATH_ARGS)
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
    // If this check fails, we just restore the URL.
    const char* orig_url = ssn->m_args.url;
    char* url_link = NULL;
    // If it does, set the instance name
    if (address)
    {
        // Let is viewable as username
        *address = '\0';
        address += sizeof("%40")-1;
        easprintf(&url_link, "https://%s/", address);
        ssn->m_args.url = url_link;
    }
    else {
        // Reset to instance url
        ssn->m_args.url = config_instance_url;
    }

    mstdnt_register_app(api,
                        &ssn->m_args,
                        request_cb_register_app,
                        path_args_data_create(req, ssn, api, NULL),
                        args_app);
}

int
content_login(PATH_ARGS)
{
    set_mstdnt_args(&ssn->m_args, ssn);

    // Check if the user logged in
    if (keystr(ssn->post.username) &&
        keystr(ssn->post.password))
    {
        register_app(PATH_ARGS_PASS);
    }
    else {
        render_login_page(req, ssn, api);
    }
}

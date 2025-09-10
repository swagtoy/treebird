/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */


#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "base_page.h"
#include "easprintf.h"
#include "cookie.h"
#include "notifications.h"
#include "string_helpers.h"
#include "config.h"
#include "local_config_set.h"
#include "account.h"
#include "cgi.h"
#include "global_cache.h"

#define BODY_STYLE "style=\"background:url('%s');\""

static void
_render_base_page(struct base_page* page,
                  FCGX_Request* req,
                  struct session* ssn,
                  struct mstdnt_notifications* notifs)
{
    PERL_STACK_INIT;

    HV* real_ssn = page->session ? page->session :
        perlify_session(ssn);
    mXPUSHs(newRV_noinc((SV*)real_ssn));
    mXPUSHs(newRV_inc((SV*)template_files));
    mXPUSHs(newSVpv(page->content, 0));

    if (notifs)
    {
        mXPUSHs(newRV_noinc(
                    (SV*)perlify_notifications(notifs->notifs, notifs->len)));
    }
    else ARG_UNDEFINED();

    // Run function
    PERL_STACK_SCALAR_CALL("base_page");
    char* dup = PERL_GET_STACK_EXIT;

    
    send_result(req, NULL, "text/html", dup, 0);
    
    tb_free(dup);
}

// Callback: render_base_page
static int
request_cb_base_page(mstdnt_request_cb_data* cb_data,
                     void* args)
{
#if 0
    struct mstdnt_notifications* notifs = MSTDNT_CB_DATA(cb_data);
    struct path_args_data* path_data = args;

    // ->data is the page data
    _render_base_page(path_data->data,
                      path_data->req,
                      path_data->ssn,
                      notifs);

    path_args_data_destroy(path_data);
#endif
    return MSTDNT_REQUEST_DONE;
}

void
render_base_page(struct base_page* page,
                 FCGX_Request* req,
                 struct session* ssn,
                 mastodont_t* api)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);

    // Fetch notification (if not iFrame)
    if (keystr(ssn->cookies.logged_in) &&
        keystr(ssn->cookies.access_token) &&
        !ssn->config.notif_embed)
    {
        struct path_args_data* path_data =
            path_args_data_create(req, ssn, api, page);
        
        mstdnt_get_notifications(
            api,
            &m_args,
            request_cb_base_page, path_data,
            (struct mstdnt_notifications_args)
            {
                .exclude_types = 0,
                .account_id = NULL,
                .exclude_visibilities = 0,
                .include_types = 0,
                .with_muted = 1,
                .max_id = NULL,
                .min_id = NULL,
                .since_id = NULL,
                .offset = 0,
                .limit = 8,
            });
    }
    else {
        // Render it as is, if IFrame is disabled
        // TODO might get account data at this point
        _render_base_page(page, req, ssn, NULL);
    }
}

void send_result(FCGX_Request* req, char* status, char* content_type, char* data, size_t data_len)
{
    if (data_len == 0) data_len = strlen(data);
#ifdef CGI_MODE
    FCGI_printf(
#else
    FCGX_FPrintF(req->out,
#endif
                 "Status: %s\r\n"
                 "Content-type: %s\r\n"
                 "Content-Length: %d\r\n\r\n",
                 status ? status : "200 OK",
                 content_type ? content_type : "text/html",
                 data_len);
#ifdef CGI_MODE
    FCGI_puts(data);
#else
    FCGX_PutStr(data, data_len, req->out);
#endif
}

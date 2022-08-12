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

#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "base_page.h"
#include "easprintf.h"
#include "cookie.h"
#include "notifications.h"
#include "string_helpers.h"
#include "../config.h"
#include "local_config_set.h"
#include "account.h"
#include <fcgi_stdio.h>
#include "global_cache.h"

// Files
#include "../static/index.ctmpl"
#include "../static/quick_login.ctmpl"
#include "../templates/main.ctt"

#define BODY_STYLE "style=\"background:url('%s');\""

void render_base_page(struct base_page* page, FCGX_Request* req, struct session* ssn, mastodont_t* api)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_notification* notifs = NULL;
    size_t notifs_len = 0;

    // Fetch notification (if not iFrame)
    if (keystr(ssn->cookies.logged_in) && keystr(ssn->cookies.access_token) &&
        !ssn->config.notif_embed)
    {
        struct mstdnt_notifications_args args = {
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
        };
        
        mastodont_get_notifications(
            api,
            &m_args,
            &args,
            &storage,
            &notifs,
            &notifs_len
            );
    }

    // Init perl stack
    perl_lock();
    dSP;
    ENTER;
    SAVETMPS;
    PUSHMARK(SP);

    if (page->session)
        mXPUSHs(newRV_inc((SV*)page->session));
    else
        mXPUSHs(newRV_inc((SV*)perlify_session(ssn)));
    XPUSHs(newRV_inc((SV*)template_files));
    XPUSHs(sv_2mortal(newSVpv(page->content, 0)));

    if (notifs && notifs_len)
    {
        AV* notifs_av = perlify_notifications(notifs, notifs_len);
        XPUSHs(sv_2mortal(newRV_inc((SV*)notifs_av)));
    }
    else XPUSHs(&PL_sv_undef);
    
    // Run function
    PUTBACK;
    call_pv("base_page", G_SCALAR);
    SPAGAIN;

    send_result(req, NULL, "text/html", POPp, 0);
    
    PUTBACK;
    FREETMPS;
    LEAVE;
    perl_unlock();
    
    mstdnt_cleanup_notifications(notifs, notifs_len);
    mastodont_storage_cleanup(&storage);
}

void send_result(FCGX_Request* req, char* status, char* content_type, char* data, size_t data_len)
{
    if (data_len == 0) data_len = strlen(data);
#ifdef SINGLE_THREADED
    printf(
#else
    FCGX_FPrintF(req->out,
#endif
                 "Status: %s\r\n"
                 "Content-type: %s\r\n"
                 "Content-Length: %d\r\n\r\n",
                 status ? status : "200 OK",
                 content_type ? content_type : "text/html",
                 data_len);
#ifdef SINGLE_THREADED
    puts(data);
#else
    FCGX_PutStr(data, data_len, req->out);
#endif
}

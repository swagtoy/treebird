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

#include "page_config.h"
#include "global_perl.h"
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "base_page.h"
#include "../config.h"
#include "easprintf.h"
#include "query.h"
#include "cookie.h"
#include "local_config_set.h"
#include "string_helpers.h"
#include "l10n.h"
#include <fcgi_stdio.h>

#define bool_checked(key) (ssn->config.key ? "checked" : "")

enum config_category
{
    CONFIG_CAT_GENERAL,
    CONFIG_CAT_APPEARANCE,
    CONFIG_CAT_ACCOUNT
};

void content_config_general(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));
    
    PERL_STACK_SCALAR_CALL("config::general");
    
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_CONFIG,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    // Cleanup
    Safefree(dup);
}


void content_config_appearance(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));
    
    PERL_STACK_SCALAR_CALL("config::appearance");
    
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_CONFIG,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    // Cleanup
    Safefree(dup);
}

void content_config(PATH_ARGS)
{
    redirect(req, REDIRECT_303, "/config/general");
}

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
#include "base_page.h"
#include "about.h"

int content_about(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    mXPUSHs(newRV_inc((SV*)session_hv));
    mXPUSHs(newRV_inc((SV*)template_files));

    PERL_STACK_SCALAR_CALL("meta::about");
    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    tb_free(dup);
}


int content_about_license(PATH_ARGS)
{
    PERL_STACK_INIT;
    HV* session_hv = perlify_session(ssn);
    XPUSHs(newRV_noinc((SV*)session_hv));
    XPUSHs(newRV_noinc((SV*)template_files));

    PERL_STACK_SCALAR_CALL("meta::license");

    char* dup = PERL_GET_STACK_EXIT;
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = dup,
        .session = session_hv,
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
    tb_free(dup);
}


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

#include "account.h"
#include "session.h"
#include "../config.h"

const char* const get_instance(struct session* ssn)
{
    return keystr(ssn->cookies.instance_url) ?
        keystr(ssn->cookies.instance_url) : config_instance_url;
}

const char* const get_token(struct session* ssn)
{
    return keystr(ssn->cookies.access_token);
}

HV* perlify_session(struct session* ssn)
{
    HV* ssn_hv = newHV();
    hvstores_int(ssn_hv, "logged_in", ssn->logged_in);
    HV* ssn_post_values = perlify_post_values(&(ssn->post));
    HV* ssn_get_values = perlify_get_values(&(ssn->query));
    HV* ssn_cookie_values = perlify_cookies(&(ssn->cookies));
    HV* acct_hv = perlify_account(&(ssn->acct));
    // Config
    HV* ssn_config = perlify_config(&(ssn->config));
    hvstores_ref(ssn_hv, "config", ssn_config);
    hvstores_ref(ssn_hv, "cookies", ssn_cookie_values);
    hvstores_ref(ssn_hv, "query", ssn_get_values);
    hvstores_ref(ssn_hv, "post", ssn_post_values);
    hvstores_ref(ssn_hv, "account", acct_hv);

    return ssn_hv;
}


/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
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

void session_cleanup(struct session* ssn)
{
    tb_free(ssn->cookies_str);
    tb_free(ssn->post_str);
    tb_free(ssn->get_str);
    free_files(&(keyfile(ssn->post.files)));
    if (ssn->logged_in) mstdnt_cleanup_account(&(ssn->acct));
    mstdnt_storage_cleanup(&(ssn->acct_storage));
    /* if (attachments) */
    /*     cleanup_media_storages(ssn, attachments); */
}

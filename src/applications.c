/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#include "applications.h"

HV* perlify_application(const struct mstdnt_app* app)
{
    if (!app) return NULL;
    
    HV* app_hv = newHV();
    hvstores_str(app_hv, "id", app->id);
    hvstores_str(app_hv, "name", app->name);
    hvstores_str(app_hv, "website", app->website);
    hvstores_str(app_hv, "redirect_uri", app->redirect_uri);
    hvstores_str(app_hv, "client_id", app->client_id);
    hvstores_str(app_hv, "client_secret", app->client_secret);
    hvstores_str(app_hv, "vapid_key", app->vapid_key);

    return app_hv;
}


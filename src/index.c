/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#include <stdlib.h>
#include "timeline.h"

int content_index(FCGX_Request* req, struct session* ssn, mastodont_t* api)
{
    // Check logins
    return content_tl_home(req, ssn, api, NULL);
}

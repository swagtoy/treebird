/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */
#include <stdlib.h>
#include "base_page.h"
#include "error.h"
#include "easprintf.h"
#include "l10n.h"

void content_not_found(FCGX_Request* req, struct session* ssn, mastodont_t* api, char* path)
{
    struct base_page b = {
        .content = "Content not found",
        .sidebar_left = NULL
    };

    render_base_page(&b, req, ssn, api);
}

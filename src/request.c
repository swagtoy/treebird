/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#include <stdlib.h>
#include "request.h"

#ifndef CGI_MODE
void finish_free_request(REQUEST_T req)
{
    FCGX_Finish_r(req);
    free(req);
}
#endif // CGI_MODE

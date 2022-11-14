/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef INDEX_H
#define INDEX_H
#include <mastodont.h>
#include "session.h"

int content_index(FCGX_Request* req, struct session* ssn, mastodont_t* api);

#endif // INDEX_H

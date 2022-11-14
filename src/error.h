/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef ERROR_H
#define ERROR_H
#include <mastodont.h>
#include <stddef.h>
#include "session.h"
#include "path.h"

void content_not_found(FCGX_Request* req, struct session* ssn, mastodont_t* api, char* path);

#endif // ERROR_H

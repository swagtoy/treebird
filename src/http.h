/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef HTTP_H
#define HTTP_H
#include "request.h"
#include "cgi.h"

#define REDIRECT_303 "303 See Other"

void redirect(REQUEST_T req, char* status, char* location);

#endif // HTTP_H

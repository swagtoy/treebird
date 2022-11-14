/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef PATH_H
#define PATH_H
#include "session.h"
#include <mastodont.h>
#include <stddef.h>
#include "env.h"
#include "cgi.h"
#include "request.h"

#define PATH_ARGS REQUEST_T req, struct session* ssn, mastodont_t* api, char** data

struct path_info
{
    char* path;
    int (*callback)(PATH_ARGS);
};

int handle_paths(
    REQUEST_T req,
    struct session* ssn,
    mastodont_t* api,
    struct path_info* paths,
    size_t paths_len);

int parse_path(REQUEST_T req,
               struct session* ssn,
               mastodont_t* api,
               struct path_info* path_info);

#endif // PATH_H

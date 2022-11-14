/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include "types.h"
#include <fcgiapp.h>

struct request_args*
request_args_create(REQUEST_T req, struct session* ssn, mastodont_t* api, void* _args)
{
    struct request_args* args = malloc(sizeof(struct request_args));
    if (!args)
        perror("request_args_create: malloc");
    args->req = req;
    args->ssn = ssn;
    args->args = _args;
    return args;
}


void request_args_cleanup(struct request_args* args)
{
    FCGX_Finish_r(args->req);
    free(args->req);
    free(args->ssn);
}


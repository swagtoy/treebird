/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef TB_TYPES_H
#define TB_TYPES_H
#include <mastodont.h>
#include "types.h"
#include "session.h"

struct request_args
{
    REQUEST_T req;
    struct session* ssn;
    mastodont_t* api;
    void* args;
};

#define DESTRUCT_TB_ARGS(_args)                                 \
    REQUEST_T req = ((struct request_args*)_args)->req;         \
    struct session* ssn = ((struct request_args*)_args)->ssn;   \
    mastodont_t* api = ((struct request_args*)_args)->api;      \
    void* args = ((struct request_args*)_args)->args;           \
    (void)req;                                                  \
    (void)ssn;                                                  \
    (void)api;                                                  \
    (void)args;  

struct request_args*
request_args_create(REQUEST_T req, struct session* ssn, mastodont_t* api, void* args);

void request_args_cleanup(struct request_args* req);

#endif // TB_TYPES_H

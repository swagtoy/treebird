/*
 * Treebird - Lightweight frontend for Pleroma
 * Copyright (C) 2022 Nekobit
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

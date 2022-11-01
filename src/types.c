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


/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#include "../config.h"
#include "helpers.h"

void set_mstdnt_args(struct mstdnt_args* args, struct session* ssn)
{
    args->url = get_instance(ssn);
    args->token = get_token(ssn);
    args->flags = MSTDNT_FLAG_NO_URI_SANITIZE | config_library_flags;
}


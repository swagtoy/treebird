/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef HELPERS_H
#define HELPERS_H
#include <mastodont.h>
#include "session.h"

void set_mstdnt_args(struct mstdnt_args* args, struct session* ssn);
void print_treebird_logo();
int debug(char const* msg, ...);

#endif /* HELPERS_H */

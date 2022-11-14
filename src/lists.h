/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef LISTS_H
#define LISTS_H
#include <stddef.h>
#include <mastodont.h>
#include "session.h"

/** Creates the main lists view */
int content_lists(PATH_ARGS);
/** Creates a list and then redirects */
int list_edit(PATH_ARGS);

/** Converts list to perl hash */
HV* perlify_list(const struct mstdnt_list* list);
    
/** Converts lists to perl array */
AV* perlify_lists(const struct mstdnt_list* lists, size_t len);

#endif // LISTS_H

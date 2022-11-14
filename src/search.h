/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef SEARCH_H
#define SEARCH_H
#include <mastodont.h>
#include "session.h"
#include "path.h"
#include "global_perl.h"

int content_search_all(PATH_ARGS);
int content_search_statuses(PATH_ARGS);
int content_search_accounts(PATH_ARGS);
int content_search_hashtags(PATH_ARGS);
HV* perlify_search_results(struct mstdnt_search_results* results);

#endif /* SEARCH_H */

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef SCROBBLE_H
#define SCROBBLE_H
#include <mastodont.h>
#include "global_perl.h"

HV* perlify_scrobble(const struct mstdnt_scrobble* const scrobble);
AV* perlify_scrobbles(const struct mstdnt_scrobble* const scrobble, size_t len);

#endif /* SCROBBLE_H */

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef APPLICATIONS_H
#define APPLICATIONS_H
#include <mastodont.h>
#include "global_perl.h"

HV* perlify_application(const struct mstdnt_app* app);

#endif /* APPLICATIONS_H */

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef LOGIN_H
#define LOGIN_H
#include <stddef.h>
#include <mastodont.h>
#include "session.h"
#include "path.h"

int content_login_oauth(PATH_ARGS);
int content_login(PATH_ARGS);

#endif // LOGIN_H

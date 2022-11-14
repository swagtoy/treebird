/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef PAGE_CONFIG_H
#define PAGE_CONFIG_H
#include <stddef.h>
#include <mastodont.h>
#include "path.h"
#include "session.h"
#include "cgi.h"

int content_config_appearance(PATH_ARGS);
int content_config_general(PATH_ARGS);
//int content_config_account(PATH_ARGS);
int content_config(PATH_ARGS);

#endif // PAGE_CONFIG_H

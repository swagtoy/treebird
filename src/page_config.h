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

#ifndef PAGE_CONFIG_H
#define PAGE_CONFIG_H
#include <stddef.h>
#include <mastodont.h>
#include "path.h"
#include "session.h"
#include "cgi.h"

void content_config_appearance(PATH_ARGS);
void content_config_general(PATH_ARGS);
//void content_config_account(PATH_ARGS);
void content_config(PATH_ARGS);

#endif // PAGE_CONFIG_H

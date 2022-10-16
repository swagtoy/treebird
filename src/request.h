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

#ifndef REQUEST_H
#define REQUEST_H
#include <pthread.h>

extern pthread_mutex_t print_mutex;

#ifdef SINGLE_THREADED
#define PRINTF(str, ...) printf(str, __VA_ARGS__)
#define PUT(str) printf(str)
#define REQUEST_T void*
#else
#define PRINTF(str, ...) do { pthread_mutex_lock(&print_mutex);  \
    FCGX_FPrintF(req->out, str, __VA_ARGS__);              \
    pthread_mutex_unlock(&print_mutex); } while (0);
#define PUT(str) do { pthread_mutex_lock(&print_mutex);  \
    FCGX_FPrintF(req->out, str); \
    pthread_mutex_unlock(&print_mutex); } while (0);
#define REQUEST_T FCGX_Request*
#endif

#endif /* REQUEST_H */

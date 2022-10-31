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

#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H
#include <mastodont.h>
#include "session.h"
#include "path.h"
#include "global_perl.h"
#include "cgi.h"

// Page contents
int content_notifications(PATH_ARGS);
int content_notifications_compact(PATH_ARGS);
int content_notifications_clear(PATH_ARGS);
int content_notifications_read(PATH_ARGS);

int api_notifications(PATH_ARGS);

HV* perlify_notification(const struct mstdnt_notification* const notif);
AV* perlify_notifications(const struct mstdnt_notification* const notif, size_t len);

#endif // NOTIFICATION_H

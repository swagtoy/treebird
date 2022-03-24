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

const char* notification_type_str(mstdnt_notification_t type);

char* construct_notification(struct mstdnt_notification* notif, int* size);
char* construct_notification_compact(struct mstdnt_notification* notif, int* size);
char* construct_notifications(struct mstdnt_notification* notifs,
                              size_t size,
                              size_t* ret_size);
char* construct_notifications_compact(struct mstdnt_notification* notifs,
                                      size_t size,
                                      size_t* ret_size);

// Page contents
void content_notifications(mastodont_t* api, char** data, size_t data_size);

#endif // NOTIFICATION_H

/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
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

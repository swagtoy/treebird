/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef TIMELINE_H
#define TIMELINE_H
#include <stddef.h>
#include <mastodont.h>
#include "path.h"
#include "session.h"
#include "base_page.h"
#include "cgi.h"
#include "request.h"

/** Wrapper for content_tl_federated */
int tl_home(REQUEST_T req, struct session* ssn, mastodont_t* api, int local);

/** Wrapper for content_tl_direct */
int tl_direct(REQUEST_T req, struct session* ssn, mastodont_t* api);

/** Wrapper for content_tl_federated */
int tl_public(REQUEST_T req, struct session* ssn, mastodont_t* api, int local, enum base_category cat);

/** Wrapper for content_tl_list */
int tl_list(REQUEST_T req, struct session* ssn, mastodont_t* api, char* list_id);

/** Wrapper for content_tl_tag */
int tl_tag(REQUEST_T req, struct session* ssn, mastodont_t* api, char* tag);

/* ------------------------------------------------ */

/** Federated timeline */
int content_tl_federated(PATH_ARGS);

/** Home timeline. Shows federated timeline if not logged in */
int content_tl_home(PATH_ARGS);

/** Direct message timeline */
int content_tl_direct(PATH_ARGS);

/** Local/instance timeline */
int content_tl_local(PATH_ARGS);

/** List timeline */
int content_tl_list(PATH_ARGS);

/** Hashtag timeline */
int content_tl_tag(PATH_ARGS);

/**
 * Used to create generic timeline content. This timeline includes other features
 * such as viewing only media, hiding muted, etc. as options on the top of the
 * timeline, so this should only be used for API's which are considered "timelines"
 * to Pleroma/Mastodon.
 *
 * @param req This request
 * @param ssn This session
 * @param api The api
 * @param storage The storage for statuses, will be cleaned up in this function, do NOT
 *                cleanup yourself.
 * @param statuses The statuses, will be cleaned up in this function, do NOT cleanup yourself.
 * @param statuses_len Length of `statuses`
 * @param cat The category to "highlight" on the sidebar
 * @param header A header that is displayed above the timeline.
 * @param show_post_box If the post box should be shown or not.
 */
void content_timeline(REQUEST_T req,
                      struct session* ssn,
                      mastodont_t* api,
                      struct mstdnt_storage* storage,
                      struct mstdnt_status* statuses,
                      size_t statuses_len,
                      enum base_category cat,
                      char* header,
                      int show_post_box,
                      int fake_timeline);

#endif // TIMELINE_H

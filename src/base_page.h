/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef BASE_PAGE_H
#define BASE_PAGE_H
#include "global_perl.h"
#include "session.h"
#include <fcgi_stdio.h>
#include <fcgiapp.h>
#include <mastodont.h>
#include "l10n.h"
#include "local_config.h"
#include "path.h"
#include <pthread.h>


enum base_category
{
    BASE_CAT_NONE,
    BASE_CAT_HOME,
    BASE_CAT_LOCAL,
    BASE_CAT_FEDERATED,
    BASE_CAT_NOTIFICATIONS,
    BASE_CAT_LISTS,
    BASE_CAT_FAVOURITES,
    BASE_CAT_BOOKMARKS,
    BASE_CAT_DIRECT,
    BASE_CAT_CHATS,
    BASE_CAT_CONFIG,
};

struct base_page
{
    enum base_category category;
    char* content;
    char* sidebar_left;
    HV* session;
};

void render_base_page(struct base_page* page, FCGX_Request* req, struct session* ssn, mastodont_t* api);

/**
 * Outputs HTML in format for CGI. This can only be called once!
 *
 * @param req The FCGI request
 * @param status The full HTTP status. if NULL, then status is "200 OK"
 * @param content_type The Content-Type to display. if NULL, assume "text/html"
 * @param data HTML content
 * @param data_len Length of data. If 0, calls strlen(data)
 */
void send_result(FCGX_Request* req, char* status, char* content_type, char* data, size_t data_len);

#endif // BASE_PAGE_H

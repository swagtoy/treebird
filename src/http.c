/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include "http.h"

#define REDIR_HTML_BEGIN "<!DOCTYPE html>" \
    "<html>" \
    "<head>" \
    "<title>Redirecting</title>" \
    "</head>" \
    "<body>"
#define REDIR_HTML_END "</body>" \
    "</html>"

void redirect(REQUEST_T req, char* status, char* location)
{
    char* loc_str = location ? location : "/";
    
    PRINTF("Status: %s\r\n"
           "Location: %s\r\n\r\n"
           REDIR_HTML_BEGIN "Redirecting to <a href=\"\">%s</a>..." REDIR_HTML_END,
           status,
           loc_str,
           loc_str);
}

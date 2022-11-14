/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef REQUEST_H
#define REQUEST_H
#include <pthread.h>

#ifdef CGI_MODE
#define PRINTF(str, ...) printf(str, __VA_ARGS__)
#define PUT(str) printf(str)
#define REQUEST_T void*
#else
#include <fcgiapp.h>
#define PRINTF(str, ...) do { FCGX_FPrintF(req->out, str, __VA_ARGS__); } while (0);
#define PUT(str) do { FCGX_FPrintF(req->out, str); } while (0);
#define REQUEST_T FCGX_Request*
#endif // CGI_MODE

void
finish_free_request(REQUEST_T req);

#endif /* REQUEST_H */

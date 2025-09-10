/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

/* #ifndef IMPORT_CGI_H */
/* #define IMPORT_CGI_H */

#ifndef NO_FCGI_DEFINES
#define 
#include <fcgi_stdio.h>
#endif // NO_FCGI

#ifndef SINGLE_THREADED
#include <fcgiapp.h>
#endif // SINGLE_THREADED

// #endif


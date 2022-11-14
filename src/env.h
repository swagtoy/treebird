/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef ENV_H
#define ENV_H

#ifdef CGI_MODE
#define GET_ENV(var, reqp) getenv(var)
#else
#define GET_ENV(var, reqp) FCGX_GetParam(var, req->envp)
#endif

#endif /* ENV_H */

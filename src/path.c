/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <string.h>
#include <stdlib.h>
#include "path.h"
#include "index.h"
#include "account.h"
#include "error.h"
#include "session.h"

int parse_path(REQUEST_T req,
               struct session* ssn,
               mastodont_t* api,
               struct path_info* path_info)
{
    int res = -1;
    int fail = 0, fin = 0;
    char* p = path_info->path + 1;
    char* p2 = GET_ENV("PATH_INFO", req) + 1;

    // Stored into data
    char* tmp = NULL;
    char** data = NULL;
    size_t size = 0;

    char* after_str = 0;
    size_t read_len;
    
    for (int i = 0, j = 0; p[j] || p2[i]; (++i, ++j))
        switch (p[j])
        {
        case ':':
            // TODO null check
            if (p[j+1] == '\0')
            {
                after_str = strchr(p2 + i, '/');
                if (!after_str)
                    after_str = p2+i + strlen(p2+i);
                fin = 1;
            }
            else
                after_str = strstr(p2 + i, "/");
            if (!after_str)
            {
                fail = 1;
                goto breakpt;
            }

            read_len = (size_t)after_str - (size_t)(p2 + i);
            // Copy in new data from the string we just read
            tmp = tb_malloc(read_len+1);
            strncpy(tmp, after_str - read_len, read_len);
            tmp[read_len] = '\0';
            // Add our new string
            data = tb_realloc(data, ++size * sizeof(tmp));
            data[size-1] = tmp;
            // Move ahead (-1 because we move again)
            i += read_len - 1;
            if (fin) goto breakpt;
            break;
        default:
            if (p2[i] == '/' && p[j] == '\0') goto breakpt;
            if (p[j] != p2[i])
            {
                fail = 1;
                goto breakpt;
            }
            break;
        }
breakpt:
    if (!fail)
    {
        res = path_info->callback(req, ssn, api, data);
    }
    else
    {
        res = -1;
    }

    // Cleanup
    for (size_t i = 0; i < size; ++i)
    {
        tb_free(data[i]);
    }
    if (data) tb_free(data);
    return res;
}

struct path_args_data*
path_args_data_create(REQUEST_T req,
                      struct session* ssn,
                      mastodont_t* api,
                      void* data)
{
    struct path_args_data* _data = malloc(sizeof(struct path_args_data));
    if (!_data)
    {
        perror("malloc");
    }
    _data->req = req;
    _data->ssn = ssn;
    _data->api = api;
    _data->data = data;

    return _data;
}

void
path_args_data_destroy(struct path_args_data* refs)
{
    session_cleanup(refs->ssn);
    FCGX_Finish_r(refs->req);
    free(refs->req);
}

int handle_paths(REQUEST_T req,
                  struct session* ssn,
                  mastodont_t* api,
                  struct path_info* paths,
                  size_t paths_len)
{
    int res;
    char* path = GET_ENV("PATH_INFO", req);
    // "default" path
    if (path == NULL || (path && strcmp(path, "/") == 0))
    {
        return content_index(req, ssn, api);
    }
    else {   // Generic path
        for (size_t i = 0; i < paths_len; ++i)
        {
            if ((res = parse_path(req, ssn, api, paths + i)) != -1)
                return;
        }

        // Fell out, return 404
        content_not_found(req, ssn, api, path);
    }
}

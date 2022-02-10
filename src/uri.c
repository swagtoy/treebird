#include <string.h>
#include "uri.h"

int parse_uri(char* dest, size_t dest_max_n, char* src)
{
    strncpy(dest, src, dest_max_n);
    char* delim = strchr(dest, '/');
    int xp = delim != NULL; /* Expression */
    if (xp || dest[strlen(dest)] == '\0')
    {
        /* Incase the second expression didn't match, check xp again */
        if (xp) *delim = '\0';
        return 0;
    }

    return 1;
}

#include <string.h>
#include "../../src/string.h"

int string_replace_test(void)
{
    char* res1 = strrepl("hello world", "wo", "swi", STRREPL_ALL);
    assert(strcmp(res1, "hello swirld") == 0);
    char* res2 = strrepl("hello world itswo the world wo", "wo", "swi", STRREPL_ALL);
    assert(strcmp(res2, "hello swirld itsswi the swirld swi") == 0);
    char* res3 = strrepl(">implying\nhuh? <><", ">", "&gt;", STRREPL_ALL);
    assert(strcmp(res3, "&gt;implying\nhuh? <&gt;<") == 0);
    char* res4 = strrepl(">lol >hi", ">", ">>>", STRREPL_ALL);
    assert(strcmp(res4, ">>>lol >>>hi") == 0);
    free(res1);
    free(res2);
    free(res3);
    free(res4);
    return 0;
}

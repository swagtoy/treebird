#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../src/string.h"

/* INFO name='Dummy test' valgrind=0 */

int main(void)
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

#include "../../src/mime.h"
#define BOUNDARY_CONTENT_T1 "multipart/form-data; boundary=---------------------------9051914041544843365972754266"
#define BOUNDARY_CONTENT_T2 "multipart/form-data; boundary=\"---------------------------9051914041544843365972754266\""
#define BOUNDARY_RES_T "---------------------------9051914041544843365972754266"

#define MULTIPART_TEST "-----------------------------9051914041544843365972754266\r\n" \
"Content-Disposition: form-data; name=\"text\"\r\n\r\n" \
"text default\r\n" \
"-----------------------------9051914041544843365972754266\r\n" \
"Content-Disposition: form-data; name=\"file1\"; filename=\"a.txt\"\r\n" \
"Content-Type: text/plain\r\n\r\n" \
"Content of a.txt.\r\n\r\n" \
"-----------------------------9051914041544843365972754266\r\n" \
"Content-Disposition: form-data; name=\"file2\"; filename\"a.html\"\r\n" \
"Content-Type: text/html\r\n\r\n" \
"<!DOCTYPE html><title>Content of a.html.</title>\r\n\r\n" \
"-----------------------------9051914041544843365972754266--"

#include <string.h>
#include <assert.h>

int mime_multipart_test(void)
{
    char* bound, *bound2;
    char* mem = get_mime_boundary(BOUNDARY_CONTENT_T2, &bound);
    char* mem2 = get_mime_boundary(BOUNDARY_CONTENT_T1, &bound2);
    assert(bound != NULL && bound2 != NULL);
    assert(strcmp(bound, BOUNDARY_RES_T) == 0 &&
           strcmp(bound2, BOUNDARY_RES_T) == 0);
    free(mem);
    free(mem2);

    
    return 0;
}

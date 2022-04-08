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

void mime_boundary_check(void)
{
    char* bound, *bound2;
    char* mem = get_mime_boundary(BOUNDARY_CONTENT_T2, &bound);
    char* mem2 = get_mime_boundary(BOUNDARY_CONTENT_T1, &bound2);
    assert(bound != NULL && bound2 != NULL);
    assert(strcmp(bound, BOUNDARY_RES_T) == 0 &&
           strcmp(bound2, BOUNDARY_RES_T) == 0);
    free(mem);
    free(mem2);
}


void form_check(void)
{
    struct http_form_info info;
    char* pos;
    char* multipart = malloc(sizeof(MULTIPART_TEST));
    
    assert(multipart != NULL);
    
    // Copy and test
    strcpy(multipart, MULTIPART_TEST);
    pos = read_form_data(BOUNDARY_RES_T, multipart, &info);

    assert(pos != NULL);
    assert(strcmp(info.name, "text") == 0);
    assert(strcmp(info.value, "text default") == 0);

    // test next value
    pos = read_form_data(BOUNDARY_RES_T, pos, &info);

    assert(pos != NULL);
    assert(strcmp(info.name, "file1") == 0);
    assert(strcmp(info.filename, "a.txt") == 0);
    assert(strcmp(info.content_type, "text/plain") == 0);
    assert(strcmp(info.value, "Content of a.txt.\r\n") == 0);

    // TODO keep testing!
    
    free(multipart);
}

int mime_multipart_test(void)
{
    mime_boundary_check();
    form_check();
    
    return 0;
}

#include "../mine.h"
#define BOUNDARY_CONTENT_T1 "Content-Type: multipart/form-data; boundary=---------------------------9051914041544843365972754266"
#define BOUNDARY_CONTENT_T2 "Content-Type: multipart/form-data; boundary=\"---------------------------9051914041544843365972754266\""
#define BOUNDARY_RES_T "---------------------------9051914041544843365972754266"

#define MULTIPART_TEST "-----------------------------9051914041544843365972754266\r\n"
"Content-Disposition: form-data; name=\"text\"\r\n\r\n"

"text default\r\n"
"-----------------------------9051914041544843365972754266\r\n"
"Content-Disposition: form-data; name=\"file1\"; filename=\"a.txt\"\r\n"
"Content-Type: text/plain\r\n\r\n"

"Content of a.txt.\r\n\r\n"

"-----------------------------9051914041544843365972754266\r\n"
"Content-Disposition: form-data; name=\"file2\"; filename\"a.html\"\r\n"
"Content-Type: text/html\r\n\r\n"

"<!DOCTYPE html><title>Content of a.html.</title>\r\n\r\n"

"-----------------------------9051914041544843365972754266--"


int mime_multipart_test(void)
{
    
    return 0;
}

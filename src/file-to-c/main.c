/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum args
{
    ARG_FILENAME = 1,
    ARG_VARIABLE
};

long filesize(FILE* file)
{
    long orig = ftell(file);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, orig, SEEK_SET);
    return size;
}

int main(int argc, char** argv)
{
    char* buf;
    FILE* file = fopen(argv[ARG_FILENAME], "rb");

    long size = filesize(file);

    if (!(buf = malloc(size)))
    {
        perror("malloc");
        return 1;
    }

    if (fread(buf, 1, size, file) != size)
    {
        fputs("Didn't read correctly!", stderr);
        free(buf);
        return 1;
    }

    fclose(file);

    printf("#ifndef __%s\n", argv[ARG_VARIABLE]);
    printf("#define __%s\n", argv[ARG_VARIABLE]);
    printf("static size_t %s_size = %ld;\n", argv[ARG_VARIABLE], size);
    printf("static const char %s[] = {", argv[ARG_VARIABLE]);
    for (size_t i = 0; i < size; ++i)
    {
        printf("0X%hhX,", buf[i]);
    }
    puts("0};\n#endif");
    
    free(buf);
    return 0;
}

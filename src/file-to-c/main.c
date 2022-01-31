/*
 * RatFE - Lightweight frontend for Pleroma
 * Copyright (C) 2022 Nekobit
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

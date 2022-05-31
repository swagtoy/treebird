/*
 * Treebird - Lightweight frontend for Pleroma
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
#include <ctype.h>

// TODO error handling

enum args
{
    ARG_FILENAME = 1,
    ARG_VARIABLE
};

enum tmpl_type
{
    TMPL_INT,
    TMPL_UINT,
    TMPL_STR,
    TMPL_STRLEN
};

struct tmpl_token
{
    enum tmpl_type type;
    char* token;
    int used; // Internal use only
};

long filesize(FILE* file)
{
    long orig = ftell(file);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, orig, SEEK_SET);
    return size;
}

void chexput(const char* buf, size_t size)
{
    for (size_t i = 0; i < size && buf; ++i)
    {
        printf("0X%hhX,", buf[i]);
    }
}

char* strnws(char* str)
{
    for (; isblank(*str); ++str);
    return str;
}

char* strwsc(char* str, char stop)
{
    for (; !isblank(*str) && *str != stop; ++str);
    return str;
}

char* tkn_typetostr(enum tmpl_type tkn)
{
    switch (tkn)
    {
    case TMPL_INT:
        return "int";
    case TMPL_STR:
        return "const char*";
    case TMPL_STRLEN:
        return "char*";
    case TMPL_UINT:
        return "unsigned";
    }
    return "";
}

enum tmpl_type tkn_type(char* str)
{
    if (strcmp(str, "string") == 0 ||
        strcmp(str, "str") == 0 ||
        strcmp(str, "%s") == 0)
        return TMPL_STR;
    else if (strcmp(str, "stringlen") == 0 ||
             strcmp(str, "strlen") == 0 ||
             strcmp(str, "%.s") == 0)
        return TMPL_STRLEN;
    else if (strcmp(str, "int") == 0 ||
             strcmp(str, "i") == 0 ||
             strcmp(str, "%d") == 0)
        return TMPL_INT;
    else if (strcmp(str, "unsigned") == 0 ||
             strcmp(str, "uint") == 0 ||
             strcmp(str, "%u") == 0)
        return TMPL_UINT;

    // TODO Real error handling
    return TMPL_INT;
}

char* parse_tmpl_token(char* buf, struct tmpl_token* tkn)
{
    tkn->used = 0;
    char* type_begin;
    char* type_end;
    char* tkn_begin;
    char* tkn_end;
    // skip {{
    buf += 2;
    type_begin = strnws(buf);
    type_end = strwsc(type_begin, ':');
    
    if (*type_end != ':') buf = strchr(buf, ':');
    else buf = type_end;
    
    *type_end = '\0';
    tkn->type = tkn_type(type_begin);

    ++buf;
    tkn_begin = strnws(buf);
    tkn_end = strwsc(tkn_begin, '}');
    
    if (*tkn_end == '}') buf = tkn_end + 2;
    else buf = strstr(buf, "}}") + 2;

    *tkn_end = '\0';
    tkn->token = tkn_begin;
    return buf;
}

void print_template(char* var, char* buf)
{
    char* buf_prev = buf;
    char* buf_curr = buf;
    // Store result
    struct tmpl_token* tokens = NULL;
    size_t tokens_len = 0;
    
    printf("#ifndef __%s\n"
           "#define __%s\n"
           "static const char data_%s[] = {", var, var, var);
    
    while (1)
    {
        buf_curr = strstr(buf_curr, "{{");
        if (!buf_curr) break;
        // Create tokens array
        tokens = realloc(tokens, sizeof(struct tmpl_token) * ++tokens_len);
        if (!tokens)
        {
            perror("realloc");
            break;
        }
        // Print up to this point
        chexput(buf_prev, buf_curr - buf_prev);
        buf_prev = buf_curr = parse_tmpl_token(buf_curr, tokens + (tokens_len-1));

        // Print type
        switch (tokens[tokens_len-1].type)
        {
        case TMPL_INT:
            // I'm lazy so we'll use this
            chexput("%d", 2);
            break;
        case TMPL_STR:
            chexput("%s", 2);
            break;
        case TMPL_STRLEN:
            chexput("%.s", 3);
            break;
        case TMPL_UINT:
            chexput("%u", 2);
            break;
        }
    }

    // Print remainder if any
    chexput(buf_prev, strlen(buf_prev));
    puts("0};");

    // Only create struct and function when there are tokens detected
    if (tokens_len)
    {
        printf("struct %s_template {", var);

        int should_print = 0;
        // Print tokens
        for (size_t i = 0; i < tokens_len; ++i)
        {
            should_print = 1;
            // Check if used
            for (size_t j = 0; j < tokens_len; ++j)
            {
                if (i != j &&
                    strcmp(tokens[i].token, tokens[j].token) == 0 &&
                    tokens[j].used)
                    should_print = 0;
            }
            if (should_print)
            {
                printf("%s %s;\n", tkn_typetostr(tokens[i].type), tokens[i].token);
                if (tokens[i].type == TMPL_STRLEN)
                    printf("unsigned %s_len;\n", tokens[i].token);
                tokens[i].used = 1;
            }
        }

        // Generate function
        printf("};\n"
               "char* tmpl_gen_%s(struct %s_template* data, unsigned* size){\n"
               "char* ret;\n"
               "unsigned s = easprintf(&ret, data_%s, ", var, var, var);
        for (size_t i = 0; i < tokens_len; ++i)
        {
            printf("data->%s", tokens[i].token);
            // No (null) strings, make them empty
            if (tokens[i].type == TMPL_STR || tokens[i].type == TMPL_STRLEN)
                printf("?data->%s:\"\"", tokens[i].token);
            fputs(i < tokens_len-1 ? ", " : "", stdout);
        }
        fputs(");\n"
              "if (size) *size = s;\n"
              "return ret;\n}", stdout);
    }

    // Done!
    puts("\n#endif");
    // Cleanup
    free(tokens);
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
    buf[size-1] = '\0';

    print_template(argv[ARG_VARIABLE], buf);



    
    free(buf);
    return 0;
}
 

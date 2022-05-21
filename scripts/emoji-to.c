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
#include <cjson/cJSON.h>

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

    
    if (argc <= 1)
    {
        fputs("Pass the file to parse!\n", stderr);
        return 1;
    }
    
    FILE* file = fopen(argv[1], "rb");

    long size = filesize(file);


    // Open and read file into buffer
    if (!(buf = malloc(size)))
    {
        perror("malloc");
        return 1;
    }
    if (fread(buf, 1, size, file) != size)
    {
        fputs("Didn't read correctly!\n", stderr);
        free(buf);
        return 1;
    }
    fclose(file);

    // Parse json
    cJSON* json = cJSON_Parse(buf);
    // Cleanup regardless of error
    free(buf);
    if (!json)
    {
        const char* error = cJSON_GetErrorPtr();
        if (error)
            fprintf(stderr, "Error parsing: %s\n", error);
        return 1;
    }

    
                                                     
    printf("#ifndef EMOJOS_H\n"
           "#define EMOJOS_H\n"
           "#include \"emoji_info.h\"\n"
           "#define emojos_size %d\n#define EMOJO_CAT_SMILEY 0\n", cJSON_GetArraySize(json));
    // Skip smileys & emotion, we already know that

    int i = 0;
    unsigned cat = 0;
    for (cJSON* groupitem = json->child; groupitem; (groupitem = groupitem->next, ++i))
    {
        cJSON* group = cJSON_GetObjectItemCaseSensitive(groupitem, "group");
        if (strcmp(group->valuestring, "Animals & Nature") == 0 && cat == 0)
        {
            cat = 1;
            printf("#define EMOJO_CAT_ANIMALS %d\n", i);
        }
        else if (strcmp(group->valuestring, "Food & Drink") == 0 && cat == 1) {
            cat = 2;
            printf("#define EMOJO_CAT_FOOD %d\n", i);
        }
        else if (strcmp(group->valuestring, "Travel & Places") == 0 && cat == 2) {
            cat = 3;
            printf("#define EMOJO_CAT_TRAVEL %d\n", i);
        }
        else if (strcmp(group->valuestring, "Activities") == 0 && cat == 3) {
            cat = 4;
            printf("#define EMOJO_CAT_ACTIVITIES %d\n", i);
        }
        else if (strcmp(group->valuestring, "Objects") == 0 && cat == 4) {
            cat = 5;
            printf("#define EMOJO_CAT_OBJECTS %d\n", i);
        }
        else if (strcmp(group->valuestring, "Symbols") == 0 && cat == 5) {
            cat = 6;
            printf("#define EMOJO_CAT_SYMBOLS %d\n", i);
        }
        else if (strcmp(group->valuestring, "Flags") == 0 && cat == 6) {
            printf("#define EMOJO_CAT_FLAGS %d\n", i);
            break;
        }
    }
    
    printf("static struct emoji_info emojos[] = {");
    i = 0;
    for (cJSON* item = json->child; item; (item = item->next, ++i))
    {
        cJSON* chr = cJSON_GetObjectItemCaseSensitive(item, "char");
        cJSON* name = cJSON_GetObjectItemCaseSensitive(item, "name");
        cJSON* group = cJSON_GetObjectItemCaseSensitive(item, "group");

        printf("{\"%s\",\"%s\" }, /*%d : %s*/\n",
               chr->valuestring,
               name->valuestring,
               i, group->valuestring);
    }
    puts("};\n#endif");

    cJSON_Delete(json);
    return 0;
}


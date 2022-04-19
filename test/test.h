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

#ifndef TEST_H
#define TEST_H

struct test
{
    char* test_name;
    int (*callback)(void);
};

int iterate_tests(struct test* tests, size_t tests_len)
{
    int status;

    for (size_t i = 0; i < tests_len; ++i)
    {
        printf("[ Test ] %04ld   %s\n", i, tests[i].test_name);
        status = tests[i].callback();
        printf("[ Test \"%s\" %s! ]\n", tests[i].test_name, !status ? "passed" : "failed");
        if (status)
            return 1;
    }
    
    return 0;
}

#endif // TEST_H

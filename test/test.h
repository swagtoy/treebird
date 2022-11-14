/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
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

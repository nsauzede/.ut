/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "ut.h"

#ifndef _WIN32
TESTMETHOD(test_intentional_memleak) {
    char *env = getenv("UT_MEMLEAK");
    int do_leak = 0;
    if (do_leak || (env && !strcmp(env, "1"))) {
        printf("%s\n", strdup("*intentional memleak*"));
    }
    ASSERT(1);
}
#endif

#include <stdint.h>

TESTCASE(Test_generic)
    TESTMETHOD(test_int) {
        ASSERT_EQ(1, 1-1*0);
    }
#define TESTUT_GENERIC_STR "some_string"
    static const char *g_str = TESTUT_GENERIC_STR;
    TESTMETHOD(test_str) {
        char l_str[] = TESTUT_GENERIC_STR;
        ASSERT((uintptr_t)g_str != (uintptr_t)l_str);
        ASSERT_EQ(g_str, l_str);
    }
    TESTMETHOD(test_ptr) {
        ASSERT_NEQ(NULL, (void *)1);
    }
    TESTMETHOD(test_ptr) {
        ASSERT_NEQ(NULL, (void*)1);
        ASSERT_EQ((void*)1, (void*)1);
    }

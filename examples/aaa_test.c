/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "basicmath.c"  // dummy dep

#include "ut.h"
TESTCASE(Test1)
    TESTMETHOD(test) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        ASSERT(1-1 == 0+1*0);
        _Static_assert(1 == 1, "hello");
        printf("This is fine.\n");
    }
    TESTMETHOD(test2) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        ASSERT(1 == 1-1*0);
        printf("This is fine.\n");
    }

TESTCASE(Test2)
    void composed_assert(int a, int b) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        ASSERT(a+1 == b+1*0);
    }
    TESTMETHOD(test) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        composed_assert(42, 43);
        printf("This is fine.\n");
    }

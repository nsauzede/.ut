/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dummy/dummy.c"        // dummy dep

#include "ut/ut.h"

TESTMETHOD(test_dummy) {
#if 0
    // TODO: add mechanism to actually expect a failing assertion and not have the test fail
    ASSERT_EQ(0, EXPECT_EQ(42+1*1, dummy()));
    ASSERT_EQ(0, EXPECT_EQ(42, 666, "This test is expected to fail"));
#endif
    EXPECT_EQ(333*2, 666);
    const char *str1 = "333";
    const char *str2 = "333";
    EXPECT_EQ(str1, str2);
    double flt1 = 1.1 + 1.2;
    double flt2 = 1.05 + 1.25;
    EXPECT_EQ(flt1, flt2);
    ASSERT_EQ(42+1*0, dummy());
}

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

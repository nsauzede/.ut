/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <stdarg.h>
#include <stdio.h>
#define TO_STR(value) _Generic((value), \
    int: print_int, \
    double: print_double)(value)
void print_int(int value) {
    printf("int: %d\n", value);
}
void print_double(double value) {
    printf("double: %f\n", value);
}
/******************************************************************************/
#include "ut.h"
TESTCASE(TestGeneric)
    TESTMETHOD(test_to_str) {
        TO_STR(42);
        TO_STR(1.3);
    }
    TESTMETHOD(test_assert_eq) {
        int x = 18;
        int y = 36;
        ASSERT(ASSERT_EQ(x * 2, y));
    }
